#include "delays.h"
#include "globals.h"
#include "pinsio.h"

static void WaitCLK(int i)
{
	if (MCUfreq == 0)
		return;
	if (proctype == PROC_TYPE_AVR) // AVR
		WaitNS(i * tCLK_AVR);
	else
		WaitNS(i * tCLK_8252);
}

uint8_t WriteReadByte(uint8_t n)
{
	uint8_t x = 0, y;

	for (y = 0; y <= 7; y++) {
		if ((n & 128) == 128)
			Send1();
		else
			Send0();
		if (proctype == PROC_TYPE_AVR)
			WaitCLK(2);
		else
			WaitCLK(1);
		ClkHi();
		if (proctype == PROC_TYPE_AVR)
			WaitCLK(4);
		else
			WaitCLK(1);
		x <<= 1;
		if (ReadBit())
			x |= 1;
		WaitCLK(1); /* */
		ClkLo();
		WaitCLK(1);
		n <<= 1;
	}
	return x;
}

void WriteByte(uint8_t n)
{
	WriteReadByte(n);
}

uint8_t ReadByte(void)
{
	uint8_t x = 0, y;

	for (y = 0; y <= 7; y++) {
		ClkHi();
		if (proctype == PROC_TYPE_AVR)
			WaitCLK(4);
		else
			WaitCLK(1);
		x <<= 1;
		if (ReadBit())
			x |= 1;
		WaitCLK(1); /* */
		ClkLo();
		WaitCLK(3);
	}
	return x;
}

void WriteBytes(const void *ptr, int len)
{
	const uint8_t *b = (const uint8_t *) ptr;
	while (len > 0) {
		WriteByte(*b);
		b++;
		len--;
	}
}

void ReadBytes(void *ptr, int len)
{
	uint8_t *b = (uint8_t *) ptr;
	while (len > 0) {
		*b = ReadByte();
		b++;
		len--;
	}
}

void WriteReadBytes(void *writeptr, void *readptr, int len)
{
	uint8_t *wb = (uint8_t *) writeptr, *rb = (uint8_t *) readptr;
	while (len > 0) {
		*rb = WriteReadByte(*wb);
		wb++;
		rb++;
		len--;
	}
}

void Sync()
{
	/* */
}

// end of file
