#include <stdint.h>

#include "isp_usersign.h"
#include "delays.h"
#include "globals.h"
#include "processors.h"
#include "spi.h"

int isp_read_user_signature_make_request(uint32_t address, void *buf)
{
	if (proctype == PROC_TYPE_S2051) {
		/* AT89S2051/4051 User Signature Bytes */
		uint8_t *data = (uint8_t *) buf;
		data[0] = 0x22;
		data[1] = address >> 8;
		data[2] = address & 0xff;
		data[3] = 0;
		return 4;
	}
	return 0;
}

uint8_t isp_read_user_signature(uint32_t address)
{
	uint8_t data[4];
	int len;

	len = isp_read_user_signature_make_request(address, data);
	if (len > 0) {
		write_bytes(data, len - 1);
		return read_byte();
	}
	return 0xff;
}

void isp_write_user_signature(uint32_t address, uint8_t value)
{
	uint8_t data[4];

	if (proctype == PROC_TYPE_S2051) {
		/* AT89S2051/4051 User Signature Bytes */
		data[0] = 0x42;
		data[1] = address >> 8;
		data[2] = address & 0xff;
		data[3] = value;
		write_bytes(data, 4);
		spi_sync();
	}
	wait_ms(Signatures[devicenr].prog_time);
}

// end of file
