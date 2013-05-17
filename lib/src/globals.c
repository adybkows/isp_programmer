#include <stdbool.h>

#include "globals.h"

TLPTpinout ISP_PINOUT_ADD = {
  LPT_OUT_STROBE, LPT_OUT_NOTUSED, LPT_OUT_AUTOLF, LPT_OUT_INIT, LPT_OUT_SELECTIN, LPT_IN_ACK, LPT_OUT_NOTUSED, false
};

TLPTpinout ISP_PINOUT_AECISP = {
  LPT_OUT_NOTUSED, LPT_OUT_NOTUSED, LPT_OUT_D4, LPT_OUT_D5, LPT_OUT_D6,
  LPT_IN_ACK, LPT_OUT_NOTUSED, false
};

TLPTpinout ISP_PINOUT_ALTERA = {
  LPT_OUT_AUTOLF, LPT_OUT_NOTUSED, LPT_OUT_D1, LPT_OUT_D6, LPT_OUT_D0,
  LPT_IN_BUSY, LPT_OUT_NOTUSED, false
};

TLPTpinout ISP_PINOUT_ATPROG = {
  LPT_OUT_NOTUSED, LPT_OUT_NOTUSED, LPT_OUT_D3, LPT_OUT_D0, LPT_OUT_D1,
  LPT_IN_ACK, LPT_OUT_NOTUSED, false
};

TLPTpinout ISP_PINOUT_SIPROG = {
  LPT_OUT_D3, LPT_OUT_D2, LPT_OUT_D7, LPT_OUT_D5, LPT_OUT_D4, LPT_IN_ACK,
  LPT_OUT_NOTUSED, false
};

TLPTpinout ISP_PINOUT_STK200 = {
  LPT_OUT_D3, LPT_OUT_D2, LPT_OUT_D7, LPT_OUT_D5, LPT_OUT_D4, LPT_IN_ACK,
  LPT_OUT_D6, false
};

TLPTpinout ISP_PINOUT_UISP = {
  LPT_OUT_NOTUSED, LPT_OUT_NOTUSED, LPT_OUT_INIT, LPT_OUT_D0, LPT_OUT_STROBE,
  LPT_IN_BUSY, LPT_OUT_NOTUSED, false
};

uint8_t devicenr = 0;

uint8_t proctype;    // processor type (AVR/51)
long lptno;          // LPT port number
uint16_t BASE;       // LPT port base address
TLPTpinout pinout;   // LPT->ISP cable pinout
long pinout_num;     // pinout type id
long flashsize;      // Flash size in bytes
long eepromsize;     // EEPROM size in bytes
long usersigsize;    // User Signature size in bytes
long MCUfreq;        // clock freq [Hz]
long tCLK_AVR;       // clock period in ns
long tCLK_8252;      // 40 clock periods in ns
bool forcedev;       // force device with name forcename
char forcename[256]; // forced device name

uint32_t autoop;     // auto operations mask

// end of file
