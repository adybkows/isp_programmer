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

void ISPEnable()
{
	uint8_t data[4];

	data[0] = 0xac;
	data[1] = 0x53;
	data[2] = 0;
	data[3] = 0;
	if (proctype == PROC_TYPE_OLD51)
		WriteReadBytes(data, data, 3);
	else
		WriteReadBytes(data, data, 4);
	Sync();
	WaitMS(30L);
}

bool ISPPollReady()
{
	uint8_t data[3] = { 0xf0, 0, 0 };

	WriteBytes(data, 3);
	return ((ReadByte() & 0x1) == 0);
}

int ISPErase()
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
			WriteBytes(data, 4);
			Sync();
		} else if (Signatures[devicenr].algo_erase == ALGO_ERASE_TWICE) {
			/* first chip erase */
			data[0] = 0xac;
			data[1] = 0x80;
			data[2] = 0;
			data[3] = 0;
			WriteBytes(data, 4);
			Sync();
			WaitMS(Signatures[devicenr].prog_time);
			/* write $FF to address $00 in the flash */
			data[0] = 0x40;
			data[1] = 0;
			data[2] = 0;
			data[3] = 0xff;
			WriteBytes(data, 4);
			Sync();
			WaitMS(Signatures[devicenr].prog_time);
			/* second chip erase */
			data[0] = 0xac;
			data[1] = 0x80;
			data[2] = 0;
			data[3] = 0;
			WriteBytes(data, 4);
			Sync();
			WaitMS(Signatures[devicenr].prog_time);
		}
	} else if (proctype == PROC_TYPE_OLD51) {
		data[0] = 0xac;
		data[1] = 0x4;
		data[2] = 0;
		WriteBytes(data, 3);
		Sync();
	} else if (proctype == PROC_TYPE_DATAFLASH) {
		if (Signatures[devicenr].algo == ALGO_SERIALFLASH) {
			return (SerialflashErase());
		}
		DataflashErase();
		return res;
	} else if (proctype == PROC_TYPE_SERIALFLASH) {
		return (SerialflashErase());
	}

	if (proctype == PROC_TYPE_NEW51) {
		WaitMS(500L);
	} else {
		WaitMS(32);
	}

	StrobeOff();
	RstOff();
	WaitMS(100L);

	return 0;
}

void ISPReadSign(void *s)
{
	uint8_t address, b, *ptr;
	uint8_t data[3];

	ptr = (uint8_t *) s;
	if (proctype == PROC_TYPE_DATAFLASH) {
		DataflashReadSign(s);
		return;
	}
	if (proctype == PROC_TYPE_SERIALFLASH) {
		SerialflashReadSign(s);
		return;
	}
	for (address = 0; address <= 2; address++) {
		b = 0xff;
		if (proctype == PROC_TYPE_AVR) {
			/* AVR */
			data[0] = 0x30;
			data[1] = 0;
			data[2] = address;
			WriteBytes(data, 3);
			b = ReadByte();
		} else if (proctype == PROC_TYPE_S8253) {
			/* AT89S8253 */
			data[0] = 0x28;
			data[1] = 0;
			data[2] = address + 0x30;
			WriteBytes(data, 3);
			b = ReadByte();
		} else if (proctype == PROC_TYPE_S2051) {
			/* AT89S2051 / AT89S4051 */
			data[0] = 0x28;
			data[1] = 0;
			data[2] = address;
			WriteBytes(data, 3);
			b = ReadByte();
		} else if (proctype == PROC_TYPE_NEW51) {
			/* AT89S51/52 */
			data[0] = 0x28;
			data[1] = address;
			data[2] = 0;
			WriteBytes(data, 3);
			b = ReadByte();
		}
		*ptr = b;
		ptr++;
	}
}

void ISPReadOscCalBytes(uint8_t *b0, uint8_t *b1, uint8_t *b2, uint8_t *b3)
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
		WriteBytes(data, 3);
		*b0 = ReadByte();
	}

	if (Signatures[devicenr].osccal > 1) {
		data[0] = 0x38;
		data[1] = 0;
		data[2] = 0x1;
		WriteBytes(data, 3);
		*b1 = ReadByte();
	}

	if (Signatures[devicenr].osccal > 2) {
		data[0] = 0x38;
		data[1] = 0;
		data[2] = 0x2;
		WriteBytes(data, 3);
		*b2 = ReadByte();
	}

	if (Signatures[devicenr].osccal > 3) {
		data[0] = 0x38;
		data[1] = 0;
		data[2] = 0x3;
		WriteBytes(data, 3);
		*b3 = ReadByte();
	}
}

void ISPReadMemoryBlock(int memtype, uint32_t address, void *buf, uint32_t len)
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
				n = ISPReadFlash_MakeRequest(address, ptr);
			else if (memtype == BUF_EEPROM)
				n = ISPReadEEPROM_MakeRequest(address, ptr);
			else if (memtype == BUF_USERSIG)
				n = ISPReadUserSign_MakeRequest(address, ptr);
			address++;
			ptr += n;
			l += n;
		}
		/* execute the request */
		WriteReadBytes(data, data, l);
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
