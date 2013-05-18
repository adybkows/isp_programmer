#ifndef MEMBUFFER_H
#define MEMBUFFER_H

#include <stdbool.h>
#include <stdint.h>

#include "processors.h"

#define BUF_FLASH           0
#define BUF_EEPROM          1
#define BUF_USERSIG         2

#define FILE_TYPE_INTELHEX  0
#define FILE_TYPE_BINARY    1

extern uint8_t flashbuffer[MAX_FLASH_SIZE];
extern uint8_t eeprombuffer[MAX_EEPROM_SIZE];
extern uint8_t usersigbuffer[MAX_USERSIG_SIZE];

void ClearBuffer(int bufid);
const char *LoadFile(int bufid, int filetype, const char *fname, uint32_t buflen, uint32_t *minaddr, uint32_t *maxaddr);
const char *SaveFile(int bufid, int filetype, const char *fname, uint32_t buflen);
const char *LoadIntelHex(int bufid, const char *fname, uint32_t buflen, uint32_t *minaddr, uint32_t *maxaddr);
const char *SaveIntelHex(int bufid, const char *fname, uint32_t buflen);
const char *LoadBinaryFile(int bufid, const char *fname, uint32_t buflen, uint32_t *minaddr, uint32_t *maxaddr);
const char *SaveBinaryFile(int bufid, const char *fname, uint32_t buflen);
int HighestUsed(int bufid, uint32_t buflen);
int LowestUsed(int bufid, uint32_t buflen);
bool IsBlockEmpty(int bufid, uint32_t startadr, uint32_t len);
bool IsIntelHex(const char *fname, int bufid);
bool IsIntelHexFilename(const char *fname);

#endif /*MEMBUFFER_H*/
