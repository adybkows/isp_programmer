#include "isp_fusebits.h"
#include "delays.h"
#include "globals.h"
#include "processors.h"
#include "spi.h"

int isp_read_fusebits(uint8_t *fblo, uint8_t *fbhi, uint8_t *fbext)
{
	uint8_t data[3];
	int res;

	if (!any_fusebits()) {
		/* no fuse bits accessible by ISP */
		return -1;
	}

	res = 0;
	*fblo = 0;
	*fbhi = 0;
	*fbext = 0;
	switch (Signatures[devicenr].algo_lb) {

		case ALGO_LB_89S8253:
		case ALGO_LB_89S2051:
			data[0] = 0x21;
			data[1] = 0;
			data[2] = 0;
			write_bytes(data, 3);
			*fblo = read_byte() & 0xf;
			break;

		case ALGO_LB_2323:
			data[0] = 0x58;
			data[1] = 0;
			data[2] = 0;
			write_bytes(data, 3);
			*fblo = read_byte() & 0x1;
			break;

		case ALGO_LB_2333:
			data[0] = 0xa0;
			data[1] = 0;
			data[2] = 0;
			write_bytes(data, 3);
			*fblo = read_byte();
			break;

		case ALGO_LB_MEGA:
		case ALGO_LB_TINY:
			if (any_fusebits_low()) {
				data[0] = 0x50;
				data[1] = 0;
				data[2] = 0;
				write_bytes(data, 3);
				*fblo = read_byte();
			}
			if (any_fusebits_high()) {
				data[0] = 0x58;
				data[1] = 0x8;
				data[2] = 0;
				write_bytes(data, 3);
				*fbhi = read_byte();
			}
			if (any_fusebits_ext()) {
				data[0] = 0x50;
				data[1] = 0x8;
				data[2] = 0;
				write_bytes(data, 3);
				*fbext = read_byte();
			}
			break;

		default: /* no ISP command to read fuse bits */
			res = -1;
			break;
	}
	return res;
}

void isp_write_low_fusebits(uint8_t fuse)
{
	uint8_t data[4];

	if (!any_fusebits_low())
		return;

	switch (Signatures[devicenr].algo_lb) {

		case ALGO_LB_89S8253:
		case ALGO_LB_89S2051:
			data[0] = 0xac;
			data[1] = 0x10 | (fuse & 0xf);
			data[2] = 0;
			data[3] = 0;
			write_bytes(data, 4);
			spi_sync();
			wait_ms(Signatures[devicenr].prog_time);
			break;

		case ALGO_LB_MEGA:
		case ALGO_LB_TINY:
			data[0] = 0xac;
			data[1] = 0xa0;
			data[2] = 0;
			data[3] = fuse;
			write_bytes(data, 4);
			spi_sync();
			wait_ms(Signatures[devicenr].prog_time);
			break;

		case ALGO_LB_2323:
			data[0] = 0xac;
			data[1] = 0xbe | (fuse & 0x1);
			data[2] = 0;
			data[3] = 0;
			write_bytes(data, 4);
			spi_sync();
			wait_ms(Signatures[devicenr].prog_time);
			break;

		case ALGO_LB_2333:
			data[0] = 0xac;
			data[1] = 0xa0 | (fuse & 0x1f);
			data[2] = 0;
			data[3] = 0;
			write_bytes(data, 4);
			spi_sync();
			wait_ms(Signatures[devicenr].prog_time);
			break;
	}
}

void isp_write_high_fusebits(uint8_t fuse)
{
	uint8_t data[4];

	if (!any_fusebits_high())
		return;

	if (Signatures[devicenr].algo != ALGO_MEGA)
		return;

	data[0] = 0xac;
	data[1] = 0xa8;
	data[2] = 0;
	data[3] = fuse;
	write_bytes(data, 4);
	spi_sync();
	wait_ms(Signatures[devicenr].prog_time);
}

void isp_write_ext_fusebits(uint8_t fuse)
{
	uint8_t data[4];

	if (!any_fusebits_ext())
		return;

	if (Signatures[devicenr].algo != ALGO_MEGA)
		return;

	data[0] = 0xac;
	data[1] = 0xa4;
	data[2] = 0;
	data[3] = fuse;
	write_bytes(data, 4);
	spi_sync();
	wait_ms(Signatures[devicenr].prog_time);
}

bool any_fusebits_low(void)
{
	int i;

	for (i = 0; i <= 7; i++) {
		if (*Signatures[devicenr].fusebitslo[i] != '\0')
			return true;
	}
	return false;
}

bool any_fusebits_high()
{
	int i;

	for (i = 0; i <= 7; i++) {
		if (*Signatures[devicenr].fusebitshi[i] != '\0')
			return true;
	}
	return false;
}

bool any_fusebits_ext()
{
	int i;

	for (i = 0; i <= 7; i++) {
		if (*Signatures[devicenr].fusebitsext[i] != '\0')
			return true;
	}
	return false;
}

bool any_fusebits()
{
	int i;

	for (i = 0; i <= 7; i++) {
		if (*Signatures[devicenr].fusebitslo[i] != '\0' || *Signatures[devicenr].fusebitshi[i] != '\0'
				|| *Signatures[devicenr].fusebitsext[i] != '\0') {
			return true;
		}
	}
	return false;
}

// end of file
