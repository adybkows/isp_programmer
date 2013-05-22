#include <stdint.h>

#include "isp_eeprom.h"
#include "delays.h"
#include "globals.h"
#include "isp.h"
#include "processors.h"
#include "membuffer.h"
#include "spi.h"

void WaitForReadyEEPROM(uint32_t address, uint8_t value)
{
	int64_t t1;
	uint8_t b;

	switch (Signatures[devicenr].algo_busy) {

		case ALGO_BUSY_WAIT:
			WaitMS(Signatures[devicenr].prog_time);
			break;

		case ALGO_BUSY_POLL_00FF:
			if (value != 0 && value != 0xff) {
				WaitMS(1);
				Tic(&t1);
				do {
					b = ISPReadEEPROM(address);
				} while (!((b == value) | (TocMS(t1) > 100)));
			} else
				WaitMS(Signatures[devicenr].prog_time);
			break;

		case ALGO_BUSY_POLL_FF:
		case ALGO_BUSY_POLL_NMSB:
			if (value != 0xff) {
				WaitMS(1);
				Tic(&t1);
				do {
					b = ISPReadEEPROM(address);
				} while (!((b == value) | (TocMS(t1) > 100)));
			} else
				WaitMS(Signatures[devicenr].prog_time);
			break;

		case ALGO_BUSY_POLL_RDYBSY:
			Tic(&t1);
			do {
			} while (!(ISPPollReady() | (TocMS(t1) > 100)));
			break;
	}
}

int ISPReadEEPROM_MakeRequest(uint32_t address, void *buf)
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

uint8_t ISPReadEEPROM(uint32_t address)
{
	uint8_t data[4];
	uint8_t res;
	int len;

	res = 0xff;
	len = ISPReadEEPROM_MakeRequest(address, data);
	if (len > 0) {
		WriteBytes(data, len - 1);
		res = ReadByte();
	}
	return res;
}

void ISPReadEEPROMPage(uint32_t address, void *buf)
{
	if (proctype == PROC_TYPE_AVR)
		ISPReadMemoryBlock(BUF_EEPROM, address, buf, Signatures[devicenr].epagesize);
}

void ISPWriteEEPROM(uint32_t address, uint8_t value)
{
	uint8_t data[4];

	if (proctype == PROC_TYPE_AVR || proctype == PROC_TYPE_NEW51 || proctype == PROC_TYPE_S8253) {
		/* AVR / AT89S8253 */
		data[0] = 0xc0;
		data[1] = address >> 8;
		data[2] = address & 0xff;
		data[3] = value;
		WriteBytes(data, 4);
		Sync();
	} else {
		/* AT89S53 / AT89S8252 */
		data[0] = (((address >> 8) & 0xff) << 3) | 6;
		data[1] = address & 0xff;
		data[2] = value;
		WriteBytes(data, 3);
		Sync();
	}
	WaitForReadyEEPROM(address, value);
}

void ISPWriteEEPROMPage(uint32_t address, const void *buf)
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
		WriteBytes(data, 4);
		ptr++;
	}
	/* Write EEPROM Memory Page */
	data[0] = 0xc2;
	data[1] = (address & (~pagemask)) >> 8;
	data[2] = (address & (~pagemask)) & 0xff;
	data[3] = 0;
	WriteBytes(data, 4);
	Sync();
	ptr = (uint8_t *) buf;
	WaitForReadyEEPROM(address, *ptr);
}

// end of file
