#include <stdint.h>

#include "dataflash.h"
#include "delays.h"
#include "globals.h"
#include "pinsio.h"
#include "processors.h"
#include "spi.h"

uint8_t dataflash_read_status(void)
{
	uint8_t res;

	chipselect_on();
	write_byte(0xd7);
	res = read_byte();
	chipselect_off();
	return res;
}

static void dataflash_read_device_id(uint8_t *b1, uint8_t *b2, uint8_t *b3)
{
	chipselect_on();
	write_byte(0x9f);
	*b1 = read_byte();
	*b2 = read_byte();
	*b3 = read_byte();
	chipselect_off();
}

static void dataflash_wait_ready()
{
	static int64_t t1 = 0;

	tic(&t1);
	do {
	} while (!(((dataflash_read_status() & 0x80) == 0x80) | (toc_ms(t1) > 100)));
}

void dataflash_erase()
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
		chipselect_on();
		data[0] = 0x50;
		data[1] = ((i << blockshift) >> 8) & 0xff;
		data[2] = (i << blockshift) & 0xff;
		data[3] = 0;
		write_bytes(data, 4);
		spi_sync();
		chipselect_off();
		dataflash_wait_ready();
	}
}

void dataflash_read_flash_page(uint32_t address, void *dataptr)
{
	uint32_t pagesize, pagenum;
	uint8_t data[8];

	if (devicenr < 1)
		return;

	pagesize = Signatures[devicenr].fpagesize;
	pagenum = address / pagesize;
	address = pagenum << Signatures[devicenr].fpage;

	chipselect_on();
	data[0] = 0xd2;
	data[1] = (address >> 16) & 0xff;
	data[2] = (address >> 8) & 0xff;
	data[3] = address & 0xff;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;
	write_bytes(data, 8);
	read_bytes(dataptr, pagesize);
	chipselect_off();
}

void dataflash_write_flash_page(uint32_t address, const void *dataptr)
{
	uint32_t pagesize, pagenum;
	uint8_t data[4];

	if (devicenr < 1)
		return;

	pagesize = Signatures[devicenr].fpagesize;
	pagenum = address / pagesize;
	address = pagenum << Signatures[devicenr].fpage;

	chipselect_on();
	data[0] = 0x82;
	data[1] = (address >> 16) & 0xff;
	data[2] = (address >> 8) & 0xff;
	data[3] = address & 0xff;
	write_bytes(data, 4);
	write_bytes(dataptr, pagesize);
	spi_sync();
	chipselect_off();
	dataflash_wait_ready();
}

void dataflash_read_signature(void *s)
{
	static uint8_t data[3] = { 0, 0, 0 };
	uint8_t b, *ptr = (uint8_t *) s;

	b = dataflash_read_status();
	dataflash_read_device_id(data, &data[1], &data[2]);
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
