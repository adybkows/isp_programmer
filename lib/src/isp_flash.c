#include <stdint.h>

#include "isp_flash.h"
#include "dataflash.h"
#include "delays.h"
#include "globals.h"
#include "isp.h"
#include "processors.h"
#include "membuffer.h"
#include "serialflash.h"
#include "spi.h"

void wait_ready_flash(uint32_t address, uint8_t value)
{
	int64_t t1;
	uint8_t b;

	switch (Signatures[devicenr].algo_busy) {

		case ALGO_BUSY_WAIT:
			wait_ms(Signatures[devicenr].prog_time);
			break;

		case ALGO_BUSY_POLL_00FF:
		case ALGO_BUSY_POLL_FF:
		case ALGO_BUSY_POLL_NMSB:
			if (value != 0xff) {
				wait_ms(1);
				tic(&t1);
				do {
					b = isp_read_flash(address);
				} while (!((b == value) | (toc_ms(t1) > 100)));
			} else
				wait_ms(Signatures[devicenr].prog_time);
			break;

		case ALGO_BUSY_POLL_RDYBSY:
			tic(&t1);
			do {
			} while (!(isp_poll_ready() | (toc_ms(t1) > 100)));
			break;
	}
}

void isp_load_extended_address(uint32_t address)
{
	uint8_t data[4];

	data[0] = 0x4d;
	data[1] = 0;
	data[2] = (address >> 17) & 0xff;
	data[3] = 0;
	write_bytes(data, 4);
	spi_sync();
}

int isp_read_flash_make_request(uint32_t address, void *buf)
{
	uint8_t *data = (uint8_t *) buf;
	int res;

	res = 0;
	if (proctype == PROC_TYPE_AVR) {
		data[0] = 0x20 | ((address & 1) << 3);
		data[1] = (address / 2) >> 8;
		data[2] = (address / 2) & 0xff;
		data[3] = 0;
		res = 4;
	} else if (proctype == PROC_TYPE_NEW51 || proctype == PROC_TYPE_S8253 || proctype == PROC_TYPE_S2051) {
		data[0] = 0x20;
		data[1] = address >> 8;
		data[2] = address & 0xff;
		data[3] = 0;
		res = 4;
	} else if (proctype == PROC_TYPE_OLD51) {
		/* AT89S53/8252 */
		data[0] = ((address >> 5) & 0xf8) | ((address >> 11) & 0x4) | 0x1;
		data[1] = address & 0xff;
		data[2] = 0;
		res = 3;
	}
	return res;
}

uint8_t isp_read_flash(uint32_t address)
{
	uint8_t data[4];
	int len;

	len = isp_read_flash_make_request(address, data);
	if (len > 0) {
		write_bytes(data, len - 1);
		return read_byte();
	}
	return 0xff;
}

void isp_read_flash_page(uint32_t address, void *buf)
{
	uint32_t pagesize, pagemask;
	uint8_t data[8];

	if (proctype == PROC_TYPE_S8253 || proctype == PROC_TYPE_S2051 || proctype == PROC_TYPE_NEW51) {
		pagesize = Signatures[devicenr].fpagesize;
		pagemask = pagesize - 1;
		data[0] = 0x30;
		data[1] = address >> 8;
		if (proctype == PROC_TYPE_NEW51) {
			write_bytes(data, 2);
		} else {
			data[2] = (address & 0xff) & (pagemask ^ 0xff);
			write_bytes(data, 3);
		}
		read_bytes(buf, pagesize);
		return;
	}
	if (proctype == PROC_TYPE_AVR) {
		pagesize = Signatures[devicenr].fpagesize;
		isp_read_memory_block(BUF_FLASH, address, buf, pagesize);
		return;
	}
	if (proctype != PROC_TYPE_DATAFLASH) {
		if (proctype == PROC_TYPE_SERIALFLASH) {
			serialflash_read_flash_page(address, buf);
		}
		return;
	}
	if (Signatures[devicenr].algo == ALGO_SERIALFLASH) {
		serialflash_read_flash_page(address, buf);
	} else {
		dataflash_read_flash_page(address, buf);
	}
}

void isp_write_flash(uint32_t address, uint8_t value)
{
	uint8_t data[4];

	if (proctype == PROC_TYPE_AVR) {
		/* AVR / ATmega */
		data[0] = 0x40 | ((address & 1) << 3);
		data[1] = (address / 2) >> 8;
		data[2] = (address / 2) & 0xff;
		data[3] = value;
		write_bytes(data, 4);
		spi_sync();
		if (Signatures[devicenr].algo == ALGO_STD) {
			wait_ready_flash(address, value);
		}
		return;
	}
	if (proctype == PROC_TYPE_NEW51 || proctype == PROC_TYPE_S8253 || proctype == PROC_TYPE_S2051) {
		/* AT89S51 / AT89S52 / AT89S8253 / AT89S2051 / AT89S4051 */
		data[0] = 0x40;
		data[1] = address >> 8;
		data[2] = address & 0xff;
		data[3] = value;
		write_bytes(data, 4);
		spi_sync();
		wait_ready_flash(address, value);
		return;
	}
	/* AT89S53 / AT89S8252 */
	data[0] = ((address >> 5) & 0xf8) | ((address >> 11) & 0x4) | 0x2;
	data[1] = address & 0xff;
	data[2] = value;
	write_bytes(data, 3);
	spi_sync();
	wait_ready_flash(address, value);
}

void isp_write_flash_page(uint32_t address, void *buf)
{
	uint32_t pagesize, pagemask, raddr;
	uint8_t data[4];
	uint8_t *ptr;

	if (proctype == PROC_TYPE_S8253 || proctype == PROC_TYPE_S2051 || proctype == PROC_TYPE_NEW51) {
		pagesize = Signatures[devicenr].fpagesize;
		pagemask = pagesize - 1;
		data[0] = 0x50;
		data[1] = address >> 8;
		if (proctype == PROC_TYPE_NEW51) {
			write_bytes(data, 2);
		} else {
			data[2] = (address & 0xff) & (pagemask ^ 0xff);
			write_bytes(data, 3);
		}
		write_bytes(buf, pagesize);
		spi_sync();
		wait_ms(Signatures[devicenr].prog_time);
		return;
	}
	if (proctype == PROC_TYPE_AVR) {
		pagesize = Signatures[devicenr].fpagesize;
		ptr = (uint8_t *) buf;
		for (raddr = address; raddr < address + pagesize; raddr++) {
			/* Load Program Memory Page */
			data[0] = 0x40 | ((raddr & 1) << 3);
			data[1] = (raddr / 2) >> 8;
			data[2] = (raddr / 2) & 0xff;
			data[3] = *ptr;
			write_bytes(data, 4);
			ptr++;
		}
		/* Write Program Memory Page */
		data[0] = 0x4c;
		data[1] = (address / 2) >> 8;
		data[2] = (address / 2) & 0xff;
		data[3] = 0;
		write_bytes(data, 4);
		spi_sync();
		ptr = (uint8_t *) buf;
		wait_ready_flash(address, *ptr);
		return;
	}
	if (proctype != PROC_TYPE_DATAFLASH) {
		if (proctype == PROC_TYPE_SERIALFLASH) {
			serialflash_write_flash_page(address, buf);
		}
		return;
	}
	if (Signatures[devicenr].algo == ALGO_SERIALFLASH) {
		serialflash_write_flash_page(address, buf);
	} else {
		dataflash_write_flash_page(address, buf);
	}
}

void isp_write_program_page(uint32_t address, uint8_t value)
{
	uint8_t data[4];

	data[0] = 0x4c;
	data[1] = (address / 2) >> 8;
	data[2] = (address / 2) & 0xff;
	data[3] = 0;
	write_bytes(data, 4);
	spi_sync();
	wait_ready_flash(address, value);
}

// end of file
