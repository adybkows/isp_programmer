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

void clear_buffer(int bufid);
const char *load_file(int bufid, int filetype, const char *fname, uint32_t buflen, uint32_t *minaddr, uint32_t *maxaddr);
const char *save_file(int bufid, int filetype, const char *fname, uint32_t buflen);
const char *load_hex_file(int bufid, const char *fname, uint32_t buflen, uint32_t *minaddr, uint32_t *maxaddr);
const char *save_hex_file(int bufid, const char *fname, uint32_t buflen);
const char *load_binary_file(int bufid, const char *fname, uint32_t buflen, uint32_t *minaddr, uint32_t *maxaddr);
const char *save_binary_file(int bufid, const char *fname, uint32_t buflen);
int highest_used(int bufid, uint32_t buflen);
int lowest_used(int bufid, uint32_t buflen);
bool is_block_empty(int bufid, uint32_t startadr, uint32_t len);
bool is_hex_file(const char *fname, int bufid);
bool is_hex_filename(const char *fname);

#endif /*MEMBUFFER_H*/
