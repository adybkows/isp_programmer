#include "delays.h"
#include "globals.h"
#include "pinsio.h"

static void wait_clk(int i)
{
	if (MCUfreq == 0)
		return;
	if (proctype == PROC_TYPE_AVR) // AVR
		wait_ns(i * tCLK_AVR);
	else
		wait_ns(i * tCLK_8252);
}

uint8_t write_read_byte(uint8_t n)
{
	uint8_t x = 0, y;

	for (y = 0; y <= 7; y++) {
		if ((n & 128) == 128)
			send_1();
		else
			send_0();
		if (proctype == PROC_TYPE_AVR)
			wait_clk(2);
		else
			wait_clk(1);
		clk_hi();
		if (proctype == PROC_TYPE_AVR)
			wait_clk(4);
		else
			wait_clk(1);
		x <<= 1;
		if (read_bit())
			x |= 1;
		wait_clk(1); /* */
		clk_lo();
		wait_clk(1);
		n <<= 1;
	}
	return x;
}

void write_byte(uint8_t n)
{
	write_read_byte(n);
}

uint8_t read_byte(void)
{
	uint8_t x = 0, y;

	for (y = 0; y <= 7; y++) {
		clk_hi();
		if (proctype == PROC_TYPE_AVR)
			wait_clk(4);
		else
			wait_clk(1);
		x <<= 1;
		if (read_bit())
			x |= 1;
		wait_clk(1); /* */
		clk_lo();
		wait_clk(3);
	}
	return x;
}

void write_bytes(const void *ptr, uint32_t len)
{
	const uint8_t *b = (const uint8_t *) ptr;
	while (len > 0) {
		write_byte(*b);
		b++;
		len--;
	}
}

void read_bytes(void *ptr, uint32_t len)
{
	uint8_t *b = (uint8_t *) ptr;
	while (len > 0) {
		*b = read_byte();
		b++;
		len--;
	}
}

void write_read_bytes(void *writeptr, void *readptr, uint32_t len)
{
	uint8_t *wb = (uint8_t *) writeptr, *rb = (uint8_t *) readptr;
	while (len > 0) {
		*rb = write_read_byte(*wb);
		wb++;
		rb++;
		len--;
	}
}

void spi_sync()
{
	/* */
}

// end of file
