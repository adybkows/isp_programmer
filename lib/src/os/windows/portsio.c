#include <stdbool.h>
#include <stdint.h>

#include "os/windows/inpout32.h"

// Read data
uint8_t InPort(uint16_t adr)
{
	return Inp32(adr);
}

// Write data
void OutPort(uint16_t adr, uint8_t y)
{
	Out32(adr,y);
}

// Returns true if the I/O sybsystem was initialized successfully
bool PortsIOInit(void)
{
	return InpOut32Init();
}

// I/O subsystem shutdown
void PortsIOShutdown(void)
{
	InpOut32Shutdown();
}

// end of file
