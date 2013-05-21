#include <stdint.h>

#include "dataflash.h"
#include "delays.h"
#include "globals.h"
#include "pinsio.h"
#include "processors.h"
#include "spi.h"

uint8_t DataflashReadStatus(void)
{
	uint8_t res;

	ChipselectOn();
	WriteByte(0xd7);
	res = ReadByte();
	ChipselectOff();
	return res;
}

static void DataflashReadDeviceID(uint8_t *b1, uint8_t *b2, uint8_t *b3)
{
	ChipselectOn();
	WriteByte(0x9f);
	*b1 = ReadByte();
	*b2 = ReadByte();
	*b3 = ReadByte();
	ChipselectOff();
}

static void DataflashWaitForReady()
{
	static int64_t t1 = 0;

	Tic(&t1);
	do {
	} while (!(((DataflashReadStatus() & 0x80) == 0x80) | (TocMS(t1) > 100)));
}

void DataflashErase()
{
	uint32_t pagesize_big, pages, blocks, blockshift, i;
	uint8_t data[4];

	if (devicenr < 1)
		return;

	pagesize_big = (1 << (Signatures[devicenr].fpage - 1)) + (1 << (Signatures[devicenr].fpage - 6));
	pages = Signatures[devicenr].fsize / pagesize_big;
	blocks = pages / 8;
	blockshift = Signatures[devicenr].fpage - 5;

	for (i = 0; i < blocks; i++) {
		ChipselectOn();
		data[0] = 0x50;
		data[1] = ((i << blockshift) >> 8) & 0xff;
		data[2] = (i << blockshift) & 0xff;
		data[3] = 0;
		WriteBytes(data, 4);
		Sync();
		ChipselectOff();
		DataflashWaitForReady();
	}
}

void DataflashReadFlashPage(uint32_t address, void *dataptr)
{
	uint32_t pagesize, pagenum;
	uint8_t data[8];

	if (devicenr < 1)
		return;

	pagesize = Signatures[devicenr].fpagesize;
	pagenum = address / pagesize;
	address = pagenum << Signatures[devicenr].fpage;

	ChipselectOn();
	data[0] = 0xd2;
	data[1] = (address >> 16) & 0xff;
	data[2] = (address >> 8) & 0xff;
	data[3] = address & 0xff;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;
	WriteBytes(data, 8);
	ReadBytes(dataptr, pagesize);
	ChipselectOff();
}

void DataflashWriteFlashPage(uint32_t address, const void *dataptr)
{
	uint32_t pagesize, pagenum;
	uint8_t data[4];

	if (devicenr < 1)
		return;

	pagesize = Signatures[devicenr].fpagesize;
	pagenum = address / pagesize;
	address = pagenum << Signatures[devicenr].fpage;

	ChipselectOn();
	data[0] = 0x82;
	data[1] = (address >> 16) & 0xff;
	data[2] = (address >> 8) & 0xff;
	data[3] = address & 0xff;
	WriteBytes(data, 4);
	WriteBytes(dataptr, pagesize);
	Sync();
	ChipselectOff();
	DataflashWaitForReady();
}

void DataflashReadSign(void *s)
{
	static uint8_t data[3] = { 0, 0, 0 };
	uint8_t b, *ptr = (uint8_t *) s;

	b = DataflashReadStatus();
	DataflashReadDeviceID(data, &data[1], &data[2]);
	if (data[0] != 0xff && data[1] != 0xff && data[2] != 0xff) {
		// memory size mask and page size bit
		ptr[0] = b & 0x3d;
	} else if (b != 0xff) {
		// memory size mask only
		ptr[0] = b & 0x3c;
	} else {
		// 0xFF and no Manufacturer/Device ID
		ptr[0] = b;
	}
	ptr[1] = data[0];
	ptr[2] = data[1];
}

// end of file
