#ifndef SERIALFLASH_H
#define SERIALFLASH_H

/* Error codes */

#define ERROR_STOP      (-1)
#define ERROR_TIMEOUT   (-2)
#define ERROR_PROGRAM   (-3)

void SerialflashUnprotectAll(void);
int SerialflashErase(void);
void SerialflashReadFlashPage(uint32_t address, void *data);
int SerialflashWriteFlashPage(uint32_t address, void *data);
void SerialflashReadSign(void *s);

#endif // SERIALFLASH_H
