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

static uint8_t serialflash_read_status()
{
	uint8_t res;

	chipselect_on();
	write_byte(AT25_CMD_READ_STATUS);
	res = read_byte();
	chipselect_off();
	return res;
}

static void serialflash_read_device_id(uint8_t *b1, uint8_t *b2)
{
	chipselect_on();
	write_byte(AT25_CMD_READ_DEVICE_ID);
	*b1 = read_byte();
	*b2 = read_byte();
	chipselect_off();
}

static void serialflash_write_enable()
{
	chipselect_on();
	write_byte(AT25_CMD_WRITE_ENABLE);
	chipselect_off();
}

/*
static void serialflash_write_disable()
{
	chipselect_on();
	write_byte(AT25_CMD_WRITE_DISABLE);
	chipselect_off();
}
*/

void serialflash_unprotect_all()
{
	int i;

	/* repeat twice (first clear SPRL, next do Global Unprotect) */
	for (i = 0; i <= 1; i++) {
		/* Write Enable */
		serialflash_write_enable();
		/* Write Status Register */
		chipselect_on();
		write_byte(AT25_CMD_WRITE_STATUS);
		write_byte(0); /* SPRL=0, Global Unprotect */
		chipselect_off();
		wait_ms(100L); /* tSR (Status Register Write Cycle Time) */
	}
}

static int serialflash_wait_ready(int timeout)
{
	uint8_t status;
	int64_t t1;

	tic(&t1);
	do {
		if (devicenr < 1)
			return ERROR_STOP;
		if (toc_ms(t1) > timeout)
			return ERROR_TIMEOUT;
		status = serialflash_read_status();
	} while ((status & STATUS_BUSY) != 0);

	if ((status & STATUS_EPE) != 0)
		return ERROR_PROGRAM;

	return 0;
}

int serialflash_erase()
{
	uint32_t i, blocks, address = 0;
	uint8_t buff[4];
	int res = 0;

	if (devicenr < 1)
		return ERROR_STOP;

	blocks = Signatures[devicenr].fsize / 65536;

	/* Global Unprotect */
	serialflash_unprotect_all();

	if (blocks >= 16) {
		for (i = 0; i < blocks; i++) {
			if (devicenr < 1)
				return ERROR_STOP;
			/* Write Enable */
			serialflash_write_enable();
			/* 64KB Block Erase */
			chipselect_on();
			buff[0] = AT25_CMD_64KB_BLOCK_ERASE;
			buff[1] = (address >> 16) & 0xff;
			buff[2] = (address >> 8) & 0xff;
			buff[3] = address & 0xff;
			write_bytes(buff, 4);
			spi_sync();
			chipselect_off();
			/* wait for ready */
			wait_ms(100);
			res = serialflash_wait_ready((Signatures[devicenr].prog_time + blocks - 1) / blocks);
			if (serialflash_erase() != 0)
				return res;
			address += 65536;
			progress_cb(address, Signatures[devicenr].fsize);
		}
		return res;
	} else {
		/* Write Enable */
		serialflash_write_enable();
		/* Chip Erase */
		chipselect_on();
		if (!memcmp(Signatures[devicenr].name, "AT25F", 5))
			write_byte(AT25F_CMD_CHIP_ERASE);
		else
			write_byte(AT25_CMD_CHIP_ERASE);
		chipselect_off();
		/* wait for ready */
		wait_ms(100);
		return (serialflash_wait_ready(Signatures[devicenr].prog_time));
	}
	return res;
}

void serialflash_read_flash_page(uint32_t address, void *data)
{
	uint8_t buff[4 /*5*/];

	if (devicenr < 1)
		return;
	chipselect_on();
	buff[0] = AT25_CMD_READ_ARRAY_SLOW; /* AT25_CMD_READ_ARRAY_FAST; */
	buff[1] = (address >> 16) & 0xff;
	buff[2] = (address >> 8) & 0xff;
	buff[3] = address & 0xff;
	/* buff[4] = 0x00; */	/* Dummy byte needed for AT25_CMD_READ_ARRAY_FAST */
	write_bytes(buff, 4 /*5*/);
	read_bytes(data, Signatures[devicenr].fpagesize);
	chipselect_off();
}

int serialflash_write_flash_page(uint32_t address, void *data)
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
		serialflash_write_enable();
		/* Page Program */
		chipselect_on();
		buff[0] = AT25_CMD_PAGE_PROGRAM;
		buff[1] = (address >> 16) & 0xff;
		buff[2] = (address >> 8) & 0xff;
		buff[3] = address & 0xff;
		write_bytes(buff, 4);
		write_bytes(b, pagelen);
		spi_sync();
		chipselect_off();
		/* wait for ready */
		res = serialflash_wait_ready(60000); /* 60ms max */
		if (res != 0)
			return res;
		address += pagelen;
		b += pagelen;
		len -= pagelen;
	} while (len != 0);
	return 0;
}

void serialflash_read_signature(void *s)
{
	uint8_t b1 = 0, b2 = 0;
	uint8_t *ptr = (uint8_t *) s;

	serialflash_read_device_id(&b1, &b2);

	if (b1 == 0x1f)
		ptr[0] = 0x25; /* constant for AT25F memories */
	else
		ptr[0] = 0xff; /* $FF and no Manufacturer/Device ID */
	ptr[1] = b1;
	ptr[2] = b2;
}

// end of file
