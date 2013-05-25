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

void WaitForReadyFlash(uint32_t address, uint8_t value)
{
	int64_t t1;
	uint8_t b;

	switch (Signatures[devicenr].algo_busy) {

		case ALGO_BUSY_WAIT:
			WaitMS(Signatures[devicenr].prog_time);
			break;

		case ALGO_BUSY_POLL_00FF:
		case ALGO_BUSY_POLL_FF:
		case ALGO_BUSY_POLL_NMSB:
			if (value != 0xff) {
				WaitMS(1);
				Tic(&t1);
				do {
					b = ISPReadFlash(address);
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

void ISPLoadExtendedAddress(uint32_t address)
{
	uint8_t data[4];

	data[0] = 0x4d;
	data[1] = 0;
	data[2] = (address >> 17) & 0xff;
	data[3] = 0;
	WriteBytes(data, 4);
	Sync();
}

int ISPReadFlash_MakeRequest(uint32_t address, void *buf)
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

uint8_t ISPReadFlash(uint32_t address)
{
	uint8_t data[4];
	int len;

	len = ISPReadFlash_MakeRequest(address, data);
	if (len > 0) {
		WriteBytes(data, len - 1);
		return ReadByte();
	}
	return 0xff;
}

void ISPReadFlashPage(uint32_t address, void *buf)
{
	uint32_t pagesize, pagemask;
	uint8_t data[8];

	if (proctype == PROC_TYPE_S8253 || proctype == PROC_TYPE_S2051 || proctype == PROC_TYPE_NEW51) {
		pagesize = Signatures[devicenr].fpagesize;
		pagemask = pagesize - 1;
		data[0] = 0x30;
		data[1] = address >> 8;
		if (proctype == PROC_TYPE_NEW51) {
			WriteBytes(data, 2);
		} else {
			data[2] = (address & 0xff) & (pagemask ^ 0xff);
			WriteBytes(data, 3);
		}
		ReadBytes(buf, pagesize);
		return;
	}
	if (proctype == PROC_TYPE_AVR) {
		pagesize = Signatures[devicenr].fpagesize;
		ISPReadMemoryBlock(BUF_FLASH, address, buf, pagesize);
		return;
	}
	if (proctype != PROC_TYPE_DATAFLASH) {
		if (proctype == PROC_TYPE_SERIALFLASH) {
			SerialflashReadFlashPage(address, buf);
		}
		return;
	}
	if (Signatures[devicenr].algo == ALGO_SERIALFLASH) {
		SerialflashReadFlashPage(address, buf);
	} else {
		DataflashReadFlashPage(address, buf);
	}
}

void ISPWriteFlash(uint32_t address, uint8_t value)
{
	uint8_t data[4];

	if (proctype == PROC_TYPE_AVR) {
		/* AVR / ATmega */
		data[0] = 0x40 | ((address & 1) << 3);
		data[1] = (address / 2) >> 8;
		data[2] = (address / 2) & 0xff;
		data[3] = value;
		WriteBytes(data, 4);
		Sync();
		if (Signatures[devicenr].algo == ALGO_STD) {
			WaitForReadyFlash(address, value);
		}
		return;
	}
	if (proctype == PROC_TYPE_NEW51 || proctype == PROC_TYPE_S8253 || proctype == PROC_TYPE_S2051) {
		/* AT89S51 / AT89S52 / AT89S8253 / AT89S2051 / AT89S4051 */
		data[0] = 0x40;
		data[1] = address >> 8;
		data[2] = address & 0xff;
		data[3] = value;
		WriteBytes(data, 4);
		Sync();
		WaitForReadyFlash(address, value);
		return;
	}
	/* AT89S53 / AT89S8252 */
	data[0] = ((address >> 5) & 0xf8) | ((address >> 11) & 0x4) | 0x2;
	data[1] = address & 0xff;
	data[2] = value;
	WriteBytes(data, 3);
	Sync();
	WaitForReadyFlash(address, value);
}

void ISPWriteFlashPage(uint32_t address, void *buf)
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
			WriteBytes(data, 2);
		} else {
			data[2] = (address & 0xff) & (pagemask ^ 0xff);
			WriteBytes(data, 3);
		}
		WriteBytes(buf, pagesize);
		Sync();
		WaitMS(Signatures[devicenr].prog_time);
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
			WriteBytes(data, 4);
			ptr++;
		}
		/* Write Program Memory Page */
		data[0] = 0x4c;
		data[1] = (address / 2) >> 8;
		data[2] = (address / 2) & 0xff;
		data[3] = 0;
		WriteBytes(data, 4);
		Sync();
		ptr = (uint8_t *) buf;
		WaitForReadyFlash(address, *ptr);
		return;
	}
	if (proctype != PROC_TYPE_DATAFLASH) {
		if (proctype == PROC_TYPE_SERIALFLASH) {
			SerialflashWriteFlashPage(address, buf);
		}
		return;
	}
	if (Signatures[devicenr].algo == ALGO_SERIALFLASH) {
		SerialflashWriteFlashPage(address, buf);
	} else {
		DataflashWriteFlashPage(address, buf);
	}
}

void ISPWriteProgramPage(uint32_t address, uint8_t value)
{
	uint8_t data[4];

	data[0] = 0x4c;
	data[1] = (address / 2) >> 8;
	data[2] = (address / 2) & 0xff;
	data[3] = 0;
	WriteBytes(data, 4);
	Sync();
	WaitForReadyFlash(address, value);
}

// end of file
