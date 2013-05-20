#ifndef DATAFLASH_H
#define DATAFLASH_H

#include <stdint.h>

uint8_t DataflashReadStatus(void);
void DataflashErase(void);
void DataflashReadFlashPage(uint32_t address, void *dataptr);
void DataflashWriteFlashPage(uint32_t address, const void *dataptr);
void DataflashReadSign(void *s);

#endif // DATAFLASH_H
