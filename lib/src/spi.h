#ifndef SPI_H
#define SPI_H

#include <stdint.h>

uint8_t write_read_byte(uint8_t n);
void write_byte(uint8_t n);
uint8_t read_byte();
void write_bytes(const void *ptr, uint32_t len);
void read_bytes(void *ptr, uint32_t len);
void write_read_bytes(void *writeptr, void *readptr, uint32_t len);
void spi_sync();

#endif // SPI_H
