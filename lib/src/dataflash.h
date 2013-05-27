#ifndef DATAFLASH_H
#define DATAFLASH_H

#include <stdint.h>

uint8_t dataflash_read_status(void);
void dataflash_erase(void);
void dataflash_read_flash_page(uint32_t address, void *dataptr);
void dataflash_write_flash_page(uint32_t address, const void *dataptr);
void dataflash_read_signature(void *s);

#endif // DATAFLASH_H
