#include <stdint.h>
#include <string.h>

#include "delays.h"
#include "globals.h"
#include "pinsio.h"
#include "processors.h"
#include "serialflash.h"
#include "spi.h"
#include "uicallbacks.h"

/* Status Register bit masks */

#define STATUS_SPRL     0x80   /* Sector Protection Registers Locked */
#define STATUS_RES      0x40
#define STATUS_EPE      0x20   /* Erase/Program Error */
#define STATUS_WPP      0x10   /* Write Protect (/WP) Pin Status */
#define STATUS_SWP      0x0c   /* Software Protection Status */
#define STATUS_WEL      0x02   /* Write Enable Latch Status */
#define STATUS_BUSY     0x01   /* Ready/Busy Status */

/* Commands */
#define AT25_CMD_READ_ARRAY_FAST   0x0b
#define AT25_CMD_READ_ARRAY_SLOW   0x03
#define AT25_CMD_CHIP_ERASE        0x60
#define AT25_CMD_64KB_BLOCK_ERASE  0xd8
#define AT25_CMD_PAGE_PROGRAM      0x02
#define AT25_CMD_WRITE_ENABLE      0x06
#define AT25_CMD_WRITE_DISABLE     0x04
#define AT25_CMD_READ_STATUS       0x05
#define AT25_CMD_WRITE_STATUS      0x01
#define AT25_CMD_READ_DEVICE_ID    0x15

#define AT25F_CMD_CHIP_ERASE       0x62

static uint8_t SerialflashReadStatus()
{
	uint8_t res;

	ChipselectOn();
	WriteByte(AT25_CMD_READ_STATUS);
	res = ReadByte();
	ChipselectOff();
	return res;
}

static void SerialflashReadDeviceID(uint8_t *b1, uint8_t *b2)
{
	ChipselectOn();
	WriteByte(AT25_CMD_READ_DEVICE_ID);
	*b1 = ReadByte();
	*b2 = ReadByte();
	ChipselectOff();
}

static void SerialflashWriteEnable()
{
	ChipselectOn();
	WriteByte(AT25_CMD_WRITE_ENABLE);
	ChipselectOff();
}

/*
static void SerialflashWriteDisable()
{
	ChipselectOn();
	WriteByte(AT25_CMD_WRITE_DISABLE);
	ChipselectOff();
}
*/

void SerialflashUnprotectAll()
{
	int i;

	/* repeat twice (first clear SPRL, next do Global Unprotect) */
	for (i = 0; i <= 1; i++) {
		/* Write Enable */
		SerialflashWriteEnable();
		/* Write Status Register */
		ChipselectOn();
		WriteByte(AT25_CMD_WRITE_STATUS);
		WriteByte(0); /* SPRL=0, Global Unprotect */
		ChipselectOff();
		WaitMS(100L); /* tSR (Status Register Write Cycle Time) */
	}
}

static int SerialflashWaitForReady(int timeout)
{
	uint8_t status;
	int64_t t1;

	Tic(&t1);
	do {
		if (devicenr < 1)
			return ERROR_STOP;
		if (TocMS(t1) > timeout)
			return ERROR_TIMEOUT;
		status = SerialflashReadStatus();
	} while ((status & STATUS_BUSY) != 0);

	if ((status & STATUS_EPE) != 0)
		return ERROR_PROGRAM;

	return 0;
}

int SerialflashErase()
{
	uint32_t i, blocks, address = 0;
	uint8_t buff[4];
	int res = 0;

	if (devicenr < 1)
		return ERROR_STOP;

	blocks = Signatures[devicenr].fsize / 65536;

	/* Global Unprotect */
	SerialflashUnprotectAll();

	if (blocks >= 16) {
		for (i = 0; i < blocks; i++) {
			if (devicenr < 1)
				return ERROR_STOP;
			/* Write Enable */
			SerialflashWriteEnable();
			/* 64KB Block Erase */
			ChipselectOn();
			buff[0] = AT25_CMD_64KB_BLOCK_ERASE;
			buff[1] = (address >> 16) & 0xff;
			buff[2] = (address >> 8) & 0xff;
			buff[3] = address & 0xff;
			WriteBytes(buff, 4);
			Sync();
			ChipselectOff();
			/* wait for ready */
			WaitMS(100);
			res = SerialflashWaitForReady((Signatures[devicenr].prog_time + blocks - 1) / blocks);
			if (SerialflashErase() != 0)
				return res;
			address += 65536;
			Progress(address, Signatures[devicenr].fsize);
		}
		return res;
	} else {
		/* Write Enable */
		SerialflashWriteEnable();
		/* Chip Erase */
		ChipselectOn();
		if (!memcmp(Signatures[devicenr].name, "AT25F", 5))
			WriteByte(AT25F_CMD_CHIP_ERASE);
		else
			WriteByte(AT25_CMD_CHIP_ERASE);
		ChipselectOff();
		/* wait for ready */
		WaitMS(100);
		return (SerialflashWaitForReady(Signatures[devicenr].prog_time));
	}
	return res;
}

void SerialflashReadFlashPage(uint32_t address, void *data)
{
	uint8_t buff[4 /*5*/];

	if (devicenr < 1)
		return;
	ChipselectOn();
	buff[0] = AT25_CMD_READ_ARRAY_SLOW; /* AT25_CMD_READ_ARRAY_FAST; */
	buff[1] = (address >> 16) & 0xff;
	buff[2] = (address >> 8) & 0xff;
	buff[3] = address & 0xff;
	/* buff[4] = 0x00; */	/* Dummy byte needed for AT25_CMD_READ_ARRAY_FAST */
	WriteBytes(buff, 4 /*5*/);
	ReadBytes(data, Signatures[devicenr].fpagesize);
	ChipselectOff();
}

int SerialflashWriteFlashPage(uint32_t address, void *data)
{
	uint32_t len = Signatures[devicenr].fpagesize;
	uint32_t pagelen = 1 << Signatures[devicenr].fpage;
	uint8_t buff[4];
	uint8_t *b = (uint8_t *) data;
	int res;

	do {
		if (devicenr < 1)
			return ERROR_STOP;
		/* Write Enable */
		SerialflashWriteEnable();
		/* Page Program */
		ChipselectOn();
		buff[0] = AT25_CMD_PAGE_PROGRAM;
		buff[1] = (address >> 16) & 0xff;
		buff[2] = (address >> 8) & 0xff;
		buff[3] = address & 0xff;
		WriteBytes(buff, 4);
		WriteBytes(b, pagelen);
		Sync();
		ChipselectOff();
		/* wait for ready */
		res = SerialflashWaitForReady(60000); /* 60ms max */
		if (res != 0)
			return res;
		address += pagelen;
		b += pagelen;
		len -= pagelen;
	} while (len != 0);
	return 0;
}

void SerialflashReadSign(void *s)
{
	uint8_t b1 = 0, b2 = 0;
	uint8_t *ptr = (uint8_t *) s;

	SerialflashReadDeviceID(&b1, &b2);

	if (b1 == 0x1f)
		ptr[0] = 0x25; /* constant for AT25F memories */
	else
		ptr[0] = 0xff;/* $FF and no Manufacturer/Device ID */
	ptr[1] = b1;
	ptr[2] = b2;
}

/* End. */
