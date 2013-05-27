#include "isp_lockbits.h"
#include "delays.h"
#include "globals.h"
#include "processors.h"
#include "spi.h"

int isp_read_lockbits(uint8_t *lb)
{
	uint8_t data[3];
	int res = 0;

	switch (Signatures[devicenr].algo_lb) {

		case ALGO_LB_2323:
			data[0] = 0x58;
			data[1] = 0;
			data[2] = 0;
			write_bytes(data, 3);
			*lb = read_byte();
			*lb = 0xf9 | (((*lb) & 0x40) >> 4) | (((*lb) & 0x80) >> 6);
			break;

		case ALGO_LB_TINY:
		case ALGO_LB_2333:
		case ALGO_LB_MEGA:
			data[0] = 0x58;
			data[1] = 0;
			data[2] = 0;
			write_bytes(data, 3);
			*lb = read_byte();
			break;

		case ALGO_LB_89S51:
			data[0] = 0x24;
			data[1] = 0;
			data[2] = 0;
			write_bytes(data, 3);
			*lb = read_byte() ^ 0xff;
			break;

		case ALGO_LB_89S8253:
		case ALGO_LB_89S2051:
			data[0] = 0x24;
			data[1] = 0;
			data[2] = 0;
			write_bytes(data, 3);
			*lb = read_byte();
			break;

		default: /* no ISP command to read lock bits */
			res = -1;
			break;
	}
	return res;
}

int isp_write_lockbits(uint8_t lb)
{
	uint8_t data[4];
	int res = 0;

	switch (Signatures[devicenr].algo_lb) {

		case ALGO_LB_89x: /* 89S53/8252 */
			data[0] = 0xac;
			data[1] = 0x7 | (lb & 0xe0);
			data[2] = 0;
			write_bytes(data, 3);
			spi_sync();
			wait_ms(Signatures[devicenr].prog_time);
			break;

		case ALGO_LB_89S51: /* 89S51/52 */
			/* Mode 1, no lock protection */
			data[0] = 0xac;
			data[1] = 0xe0;
			data[2] = 0;
			data[3] = 0;
			write_bytes(data, 4);
			spi_sync();
			wait_ms(Signatures[devicenr].prog_time);
			if ((lb & 0x4) == 0) {
				/* Mode 2, lock bit 1 activated */
				data[0] = 0xac;
				data[1] = 0xe1;
				data[2] = 0;
				data[3] = 0;
				write_bytes(data, 4);
				spi_sync();
				wait_ms(Signatures[devicenr].prog_time);
			}
			if ((lb & 0x8) == 0) {
				/* Mode 3, lock bit 2 activated */
				data[0] = 0xac;
				data[1] = 0xe2;
				data[2] = 0;
				data[3] = 0;
				write_bytes(data, 4);
				spi_sync();
				wait_ms(Signatures[devicenr].prog_time);
			}
			if ((lb & 0x10) == 0) {
				/* Mode 4, lock bit 3 activated */
				data[0] = 0xac;
				data[1] = 0xe3;
				data[2] = 0;
				data[3] = 0;
				write_bytes(data, 4);
				spi_sync();
				wait_ms(Signatures[devicenr].prog_time);
			}
			break;

		case ALGO_LB_STD:
		case ALGO_LB_TINY:
		case ALGO_LB_2323:
		case ALGO_LB_2333: /* AVR */
			data[0] = 0xac;
			data[1] = 0xf9 | (lb & 0x6);
			data[2] = 0;
			data[3] = 0;
			write_bytes(data, 4);
			spi_sync();
			wait_ms(Signatures[devicenr].prog_time);
			break;

		case ALGO_LB_MEGA: /* ATmega */
			data[0] = 0xac;
			data[1] = 0xe0;
			data[2] = 0;
			data[3] = 0xc0 | (lb & 0x3f);
			write_bytes(data, 4);
			spi_sync();
			wait_ms(Signatures[devicenr].prog_time);
			break;

		case ALGO_LB_89S8253:
		case ALGO_LB_89S2051: /* AT89S8253, AT89S2051/4051 */
			data[0] = 0xac;
			data[1] = 0xe0 | (lb & 0x7);
			data[2] = 0;
			data[3] = 0;
			write_bytes(data, 4);
			spi_sync();
			wait_ms(Signatures[devicenr].prog_time);
			break;

		default: /* no ISP command to write lock bits */
			res = -1;
			break;
	}
	return res;
}

bool any_lockbits()
{
	int i;

	for (i = 0; i <= 7; i++) {
		if (*Signatures[devicenr].lockbits[i] != '\0')
			return true;
	}
	return false;
}

// end of file
