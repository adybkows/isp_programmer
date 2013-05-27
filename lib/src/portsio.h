#ifndef PORTSIO_H
#define PORTSIO_H

#include <stdbool.h>
#include <stdint.h>

// Read data
uint8_t in_port(uint16_t adr);

// Write data
void out_port(uint16_t adr, uint8_t y);

// Returns true if the I/O sybsystem was initialized successfully
bool ports_io_init(void);

// I/O subsystem shutdown
void ports_io_shutdown(void);

#endif // PORTSIO_H
