#include <stdbool.h>
#include <stdint.h>

#include "isp.h"
#include "dataflash.h"
#include "delays.h"
#include "globals.h"
#include "isp_eeprom.h"
#include "isp_flash.h"
#include "isp_usersign.h"
#include "processors.h"
#include "membuffer.h"
#include "pinsio.h"
#include "serialflash.h"
#include "spi.h"

void isp_enable()
{
	uint8_t data[4];

	data[0] = 0xac;
	data[1] = 0x53;
	data[2] = 0;
	data[3] = 0;
	if (proctype == PROC_TYPE_OLD51)
		write_read_bytes(data, data, 3);
	else
		write_read_bytes(data, data, 4);
	spi_sync();
	wait_ms(30L);
}

bool isp_poll_ready()
{
	uint8_t data[3] = { 0xf0, 0, 0 };

	write_bytes(data, 3);
	return ((read_byte() & 0x1) == 0);
}

int isp_erase()
{
	uint8_t data[4];
	int res = 0;

	if (proctype == PROC_TYPE_AVR || proctype == PROC_TYPE_NEW51 || proctype == PROC_TYPE_S8253
			|| proctype == PROC_TYPE_S2051) {
		if (Signatures[devicenr].algo_erase == ALGO_ERASE_STD) {
			data[0] = 0xac;
			data[1] = 0x80;
			data[2] = 0;
			data[3] = 0;
			write_bytes(data, 4);
			spi_sync();
		} else if (Signatures[devicenr].algo_erase == ALGO_ERASE_TWICE) {
			/* first chip erase */
			data[0] = 0xac;
			data[1] = 0x80;
			data[2] = 0;
			data[3] = 0;
			write_bytes(data, 4);
			spi_sync();
			wait_ms(Signatures[devicenr].prog_time);
			/* write $FF to address $00 in the flash */
			data[0] = 0x40;
			data[1] = 0;
			data[2] = 0;
			data[3] = 0xff;
			write_bytes(data, 4);
			spi_sync();
			wait_ms(Signatures[devicenr].prog_time);
			/* second chip erase */
			data[0] = 0xac;
			data[1] = 0x80;
			data[2] = 0;
			data[3] = 0;
			write_bytes(data, 4);
			spi_sync();
			wait_ms(Signatures[devicenr].prog_time);
		}
	} else if (proctype == PROC_TYPE_OLD51) {
		data[0] = 0xac;
		data[1] = 0x4;
		data[2] = 0;
		write_bytes(data, 3);
		spi_sync();
	} else if (proctype == PROC_TYPE_DATAFLASH) {
		if (Signatures[devicenr].algo == ALGO_SERIALFLASH) {
			return (serialflash_erase());
		}
		dataflash_erase();
		return res;
	} else if (proctype == PROC_TYPE_SERIALFLASH) {
		return (serialflash_erase());
	}

	if (proctype == PROC_TYPE_NEW51) {
		wait_ms(500L);
	} else {
		wait_ms(32);
	}

	strobe_off();
	reset_off();
	wait_ms(100L);

	return 0;
}

void isp_read_signature(void *s)
{
	uint8_t address, b, *ptr;
	uint8_t data[3];

	ptr = (uint8_t *) s;
	if (proctype == PROC_TYPE_DATAFLASH) {
		dataflash_read_signature(s);
		return;
	}
	if (proctype == PROC_TYPE_SERIALFLASH) {
		serialflash_read_signature(s);
		return;
	}
	for (address = 0; address <= 2; address++) {
		b = 0xff;
		if (proctype == PROC_TYPE_AVR) {
			/* AVR */
			data[0] = 0x30;
			data[1] = 0;
			data[2] = address;
			write_bytes(data, 3);
			b = read_byte();
		} else if (proctype == PROC_TYPE_S8253) {
			/* AT89S8253 */
			data[0] = 0x28;
			data[1] = 0;
			data[2] = address + 0x30;
			write_bytes(data, 3);
			b = read_byte();
		} else if (proctype == PROC_TYPE_S2051) {
			/* AT89S2051 / AT89S4051 */
			data[0] = 0x28;
			data[1] = 0;
			data[2] = address;
			write_bytes(data, 3);
			b = read_byte();
		} else if (proctype == PROC_TYPE_NEW51) {
			/* AT89S51/52 */
			data[0] = 0x28;
			data[1] = address;
			data[2] = 0;
			write_bytes(data, 3);
			b = read_byte();
		}
		*ptr = b;
		ptr++;
	}
}

void isp_read_osc_cal_bytes(uint8_t *b0, uint8_t *b1, uint8_t *b2, uint8_t *b3)
{
	uint8_t data[3];

	*b0 = 0;
	*b1 = 0;
	*b2 = 0;
	*b3 = 0;

	if (Signatures[devicenr].osccal > 0) {
		data[0] = 0x38;
		data[1] = 0;
		data[2] = 0;
		write_bytes(data, 3);
		*b0 = read_byte();
	}

	if (Signatures[devicenr].osccal > 1) {
		data[0] = 0x38;
		data[1] = 0;
		data[2] = 0x1;
		write_bytes(data, 3);
		*b1 = read_byte();
	}

	if (Signatures[devicenr].osccal > 2) {
		data[0] = 0x38;
		data[1] = 0;
		data[2] = 0x2;
		write_bytes(data, 3);
		*b2 = read_byte();
	}

	if (Signatures[devicenr].osccal > 3) {
		data[0] = 0x38;
		data[1] = 0;
		data[2] = 0x3;
		write_bytes(data, 3);
		*b3 = read_byte();
	}
}

void isp_read_memory_block(int memtype, uint32_t address, void *buf, uint32_t len)
{
	uint8_t data[4097];
	uint8_t *ptr, *bbuf;
	int cnt, i, n, l;

	n = 0;
	bbuf = (uint8_t *) buf;
	while (len > 0) {
		/* calculate block size */
		if (len > 1024)
			cnt = 1024;
		else
			cnt = len;
		/* create IN/OUT transmission request */
		ptr = data;
		l = 0;
		for (i = 1; i <= cnt; i++) {
			if (memtype == BUF_FLASH)
				n = isp_read_flash_make_request(address, ptr);
			else if (memtype == BUF_EEPROM)
				n = isp_read_eeprom_make_request(address, ptr);
			else if (memtype == BUF_USERSIG)
				n = isp_read_user_signature_make_request(address, ptr);
			address++;
			ptr += n;
			l += n;
		}
		/* execute the request */
		write_read_bytes(data, data, l);
		/* copy received data to the buffer */
		for (i = 1; i <= cnt; i++) {
			ptr = bbuf;
			*ptr = data[i * n - 1];
			bbuf++;
		}
		len -= cnt;
	}
}

// end of file
