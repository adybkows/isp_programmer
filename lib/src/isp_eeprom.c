#include <stdint.h>

#include "isp_eeprom.h"
#include "delays.h"
#include "globals.h"
#include "isp.h"
#include "processors.h"
#include "membuffer.h"
#include "spi.h"

void wait_ready_eeprom(uint32_t address, uint8_t value)
{
	int64_t t1;
	uint8_t b;

	switch (Signatures[devicenr].algo_busy) {

		case ALGO_BUSY_WAIT:
			wait_ms(Signatures[devicenr].prog_time);
			break;

		case ALGO_BUSY_POLL_00FF:
			if (value != 0 && value != 0xff) {
				wait_ms(1);
				tic(&t1);
				do {
					b = isp_read_eeprom(address);
				} while (!((b == value) | (toc_ms(t1) > 100)));
			} else
				wait_ms(Signatures[devicenr].prog_time);
			break;

		case ALGO_BUSY_POLL_FF:
		case ALGO_BUSY_POLL_NMSB:
			if (value != 0xff) {
				wait_ms(1);
				tic(&t1);
				do {
					b = isp_read_eeprom(address);
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

int isp_read_eeprom_make_request(uint32_t address, void *buf)
{
	uint8_t *data = (uint8_t *) buf;
	int res;

	res = 0;
	if (proctype == PROC_TYPE_AVR || proctype == PROC_TYPE_NEW51 || proctype == PROC_TYPE_S8253) {
		data[0] = 0xa0;
		data[1] = address >> 8;
		data[2] = address & 0xff;
		data[3] = 0;
		res = 4;
	} else if (proctype == PROC_TYPE_OLD51) {
		/* AT89S53 / AT89S8252 */
		data[0] = (((address >> 8) & 0xff) << 3) | 5;
		data[1] = address & 0xff;
		data[2] = 0;
		res = 3;
	}
	return res;
}

uint8_t isp_read_eeprom(uint32_t address)
{
	uint8_t data[4];
	uint8_t res;
	int len;

	res = 0xff;
	len = isp_read_eeprom_make_request(address, data);
	if (len > 0) {
		write_bytes(data, len - 1);
		res = read_byte();
	}
	return res;
}

void isp_read_eeprom_page(uint32_t address, void *buf)
{
	if (proctype == PROC_TYPE_AVR)
		isp_read_memory_block(BUF_EEPROM, address, buf, Signatures[devicenr].epagesize);
}

void isp_write_eeprom(uint32_t address, uint8_t value)
{
	uint8_t data[4];

	if (proctype == PROC_TYPE_AVR || proctype == PROC_TYPE_NEW51 || proctype == PROC_TYPE_S8253) {
		/* AVR / AT89S8253 */
		data[0] = 0xc0;
		data[1] = address >> 8;
		data[2] = address & 0xff;
		data[3] = value;
		write_bytes(data, 4);
		spi_sync();
	} else {
		/* AT89S53 / AT89S8252 */
		data[0] = (((address >> 8) & 0xff) << 3) | 6;
		data[1] = address & 0xff;
		data[2] = value;
		write_bytes(data, 3);
		spi_sync();
	}
	wait_ready_eeprom(address, value);
}

void isp_write_eeprom_page(uint32_t address, const void *buf)
{
	uint32_t pagesize, pagemask, raddr;
	uint8_t data[4];
	uint8_t *ptr;

	if (proctype != PROC_TYPE_AVR)
		return;
	pagesize = Signatures[devicenr].epagesize;
	pagemask = pagesize - 1;
	ptr = (uint8_t *) buf;
	for (raddr = address; raddr < address + pagesize; raddr++) {
		/* Load EEPROM Memory Page */
		data[0] = 0xc1;
		data[1] = (raddr & pagemask) >> 8;
		data[2] = (raddr & pagemask) & 0xff;
		data[3] = *ptr;
		write_bytes(data, 4);
		ptr++;
	}
	/* Write EEPROM Memory Page */
	data[0] = 0xc2;
	data[1] = (address & (~pagemask)) >> 8;
	data[2] = (address & (~pagemask)) & 0xff;
	data[3] = 0;
	write_bytes(data, 4);
	spi_sync();
	ptr = (uint8_t *) buf;
	wait_ready_eeprom(address, *ptr);
}

// end of file
