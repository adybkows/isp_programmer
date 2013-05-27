#ifndef INPOUT32_H
#define INPOUT32_H

#include <stdbool.h>
#include <stdint.h>

// Driver initialization
bool inpout32_init(void);

// Driver shutdown
void inpout32_shutdown(void);

// Write data
void Out32(uint16_t port_address, uint8_t data);

// Read data
uint8_t Inp32(uint16_t port_address);

// Returns true if the InpOut driver was opened successfully
bool IsInpOutDriverOpen(void);

// Returns true if the OS is 64bit (x64) Windows.
bool IsXP64Bit(void);

#endif // INPOUT32_H
