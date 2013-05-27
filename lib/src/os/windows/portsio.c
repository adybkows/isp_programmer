#include <stdbool.h>
#include <stdint.h>

#include "os/windows/inpout32.h"

// Read data
uint8_t in_port(uint16_t adr)
{
	return Inp32(adr);
}

// Write data
void out_port(uint16_t adr, uint8_t y)
{
	Out32(adr,y);
}

// Returns true if the I/O sybsystem was initialized successfully
bool ports_io_init(void)
{
	return inpout32_init();
}

// I/O subsystem shutdown
void ports_io_shutdown(void)
{
	inpout32_shutdown();
}

// end of file
