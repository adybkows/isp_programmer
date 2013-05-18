#ifndef PORTSIO_H
#define PORTSIO_H

#include <stdbool.h>
#include <stdint.h>

// Read data
uint8_t InPort(uint16_t adr);

// Write data
void OutPort(uint16_t adr, uint8_t y);

// Returns true if the I/O sybsystem was initialized successfully
bool PortsIOInit(void);

// I/O subsystem shutdown
void PortsIOShutdown(void);

#endif // PORTSIO_H
