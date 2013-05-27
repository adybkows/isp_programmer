#ifndef SERIALFLASH_H
#define SERIALFLASH_H

/* Error codes */

#define ERROR_STOP      (-1)
#define ERROR_TIMEOUT   (-2)
#define ERROR_PROGRAM   (-3)

void serialflash_unprotect_all(void);
int serialflash_erase(void);
void serialflash_read_flash_page(uint32_t address, void *data);
int serialflash_write_flash_page(uint32_t address, void *data);
void serialflash_read_signature(void *s);

#endif // SERIALFLASH_H
