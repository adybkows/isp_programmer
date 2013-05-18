#ifndef SPI_H
#define SPI_H

#include <stdint.h>

uint8_t WriteReadByte(uint8_t n);
void WriteByte(uint8_t n);
uint8_t ReadByte();
void WriteBytes(void *ptr, int len);
void ReadBytes(void *ptr, int len);
void WriteReadBytes(void *writeptr, void *readptr, int len);
void Sync();

#endif // SPI_H
