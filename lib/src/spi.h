#ifndef SPI_H
#define SPI_H

#include <stdint.h>

uint8_t WriteReadByte(uint8_t n);
void WriteByte(uint8_t n);
uint8_t ReadByte();
void WriteBytes(const void *ptr, uint32_t len);
void ReadBytes(void *ptr, uint32_t len);
void WriteReadBytes(void *writeptr, void *readptr, uint32_t len);
void Sync();

#endif // SPI_H
