#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t strobe1;   // out
  uint8_t strobe2;   // out
  uint8_t reset;     // out
  uint8_t mosi;      // out
  uint8_t sck;       // out
  uint8_t miso;      // in
  uint8_t led;       // out
  bool resetinv;     // is RESET inverted
} TLPTpinout;

#define DataPort        0   // offset from BASE
#define StatusPort      1   // offset from BASE
#define ControlPort     2   // offset from BASE

// processor/memory types
#define PROC_TYPE_AVR          0   // AT90, ATmega, ATtiny
#define PROC_TYPE_NEW51        1   // AT89[L]S51/52
#define PROC_TYPE_OLD51        2   // AT89S53/8252
#define PROC_TYPE_S8253        3   // AT89S8253
#define PROC_TYPE_S2051        4   // AT89S2051/4051
#define PROC_TYPE_DATAFLASH    5   // AT25/26/45DB
#define PROC_TYPE_SERIALFLASH  6   // AT25F
#define PROC_TYPE_I2C_BUS      7   // I2C memories

// ISP operations
#define OP_READSIG           1          // read signature
#define OP_ERASE             2          // erase device
#define OP_BLANKCHECKFLASH   4          // blank check Flash
#define OP_BLANKCHECKEEPROM  8          // blank check EEPROM
#define OP_READFLASH         0x10       // read Flash
#define OP_WRITEFLASH        0x20       // write Flash
#define OP_VERIFYFLASH       0x40       // verify Flash
#define OP_READEEPROM        0x80       // read EEPROM
#define OP_WRITEEEPROM       0x100      // write EEPROM
#define OP_VERIFYEEPROM      0x200      // verify EEPROM
#define OP_READUSERSIG       0x400      // read user signature
#define OP_WRITEUSERSIG      0x800      // write user signature
#define OP_VERIFYUSERSIG     0x1000     // verify user signature
#define OP_READFUSE          0x2000     // read fuse bits
#define OP_WRITEFUSE         0x4000     // write fuse bits
#define OP_VERIFYFUSE        0x8000L    // verify fuse bits
#define OP_READLOCK          0x10000L   // read lock bits
#define OP_WRITELOCK         0x20000L   // write lock bits
#define OP_VERIFYLOCK        0x40000L   // verify lock bits

// LPT outputs
#define LPT_OUT_STROBE    0   // pin 1
#define LPT_OUT_D0        1   // pin 2
#define LPT_OUT_D1        2   // pin 3
#define LPT_OUT_D2        3   // pin 4
#define LPT_OUT_D3        4   // pin 5
#define LPT_OUT_D4        5   // pin 6
#define LPT_OUT_D5        6   // pin 7
#define LPT_OUT_D6        7   // pin 8
#define LPT_OUT_D7        8   // pin 9
#define LPT_OUT_AUTOLF    9   // pin 14
#define LPT_OUT_INIT      10  // pin 16
#define LPT_OUT_SELECTIN  11  // pin 17
#define LPT_OUT_NOTUSED   12

// LPT inputs
#define LPT_IN_ACK        0   // pin 10
#define LPT_IN_BUSY       1   // pin 11
#define LPT_IN_PAPEREND   2   // pin 12
#define LPT_IN_SELECT     3   // pin 13
#define LPT_IN_ERROR      4   // pin 15

// cable pinouts
#define LPT_PINOUT_ADD     0
#define LPT_PINOUT_AECISP  1
#define LPT_PINOUT_ALTERA  2
#define LPT_PINOUT_ATPROG  3
#define LPT_PINOUT_SIPROG  4
#define LPT_PINOUT_STK200  5
#define LPT_PINOUT_UISP    6
#define LPT_PINOUT_CUSTOM  7

extern TLPTpinout ISP_PINOUT_ADD;
extern TLPTpinout ISP_PINOUT_AECISP;
extern TLPTpinout ISP_PINOUT_ALTERA;
extern TLPTpinout ISP_PINOUT_ATPROG;
extern TLPTpinout ISP_PINOUT_SIPROG;
extern TLPTpinout ISP_PINOUT_STK200;
extern TLPTpinout ISP_PINOUT_UISP;

extern uint8_t devicenr;

extern uint8_t proctype;    // processor type (AVR/51)
extern long lptno;          // LPT port number
extern uint16_t BASE;       // LPT port base address
extern TLPTpinout pinout;   // LPT->ISP cable pinout
extern long pinout_num;     // pinout type id
extern long flashsize;      // Flash size in bytes
extern long eepromsize;     // EEPROM size in bytes
extern long usersigsize;    // User Signature size in bytes
extern long MCUfreq;        // clock freq [Hz]
extern long tCLK_AVR;       // clock period in ns
extern long tCLK_8252;      // 40 clock periods in ns
extern bool forcedev;       // force device with name forcename
extern char forcename[256]; // forced device name

extern uint32_t autoop;     // auto operations mask

#endif // GLOBALS_H
