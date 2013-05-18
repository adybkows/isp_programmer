#ifndef PROCESSORS_H
#define PROCESSORS_H

#include <stdint.h>

typedef char TLockBits[8][6];

typedef struct {
  uint8_t b0, b1, b2;      // Signature bytes
  const char *name;        // Chip name
  uint8_t proctype;        // Processor type: PROC_TYPE_xxx
  long fsize;              // Flash size in bytes
  long esize;              // EEPROM size in bytes
  long usigsize;           // User Signature size in bytes
  uint8_t fpage;           // Flash page bits
  long fpagesize;          // Flash page size in bytes, usually 2^fpage
  uint8_t epage;           // EEPROM page bits
  long epagesize;          // EEPROM page size in bytes, usually 2^epage
  uint8_t osccal;          // Number of oscillator calibration bytes
  uint8_t algo;            // Programming algorithm
  uint8_t algo_erase;      // Erasing algorithm
  uint8_t algo_lb;         // Lock bits and fuses reading/programming algorithm
  uint8_t algo_busy;       // Busy check algorithm
  long prog_time;          // Time in ms to wait after programming
  const char *lockbits[8];    // LSB to MSB
  const char *fusebitslo[8];  // LSB to MSB
  const char *fusebitshi[8];  // LSB to MSB
  const char *fusebitsext[8]; // LSB to MSB
} TSignature;

#define ALGO_STD         1
#define ALGO_MEGA        2
#define ALGO_DATAFLASH   3
#define ALGO_SERIALFLASH 4
#ifdef I2C_SUPPORT
	#define ALGO_AT24_EEPROM 5
#endif

#define ALGO_ERASE_STD   0
#define ALGO_ERASE_TWICE 1

#define ALGO_LB_NONE     0
#define ALGO_LB_STD      1
#define ALGO_LB_TINY     2
#define ALGO_LB_2323     3
#define ALGO_LB_2333     4
#define ALGO_LB_MEGA     5
#define ALGO_LB_89x      6
#define ALGO_LB_89S51    7
#define ALGO_LB_89S8253  8
#define ALGO_LB_89S2051  9

#define ALGO_BUSY_WAIT        1
#define ALGO_BUSY_POLL_00FF   2
#define ALGO_BUSY_POLL_FF     3
#define ALGO_BUSY_POLL_NMSB   4
#define ALGO_BUSY_POLL_RDYBSY 5
#define ALGO_BUSY_DATAFLASH   6
#define ALGO_BUSY_SERIALFLASH 7
#ifdef I2C_SUPPORT
	#define ALGO_BUSY_AT24_EEPROM 8
#endif

#define MAX_FLASH_SIZE    17301504L   // AT45CS1282 (16.5MB)
#define MAX_EEPROM_SIZE   4096
#define MAX_USERSIG_SIZE  32

#define DEVICE_UNKNOWN  0
#define DEVICE_AT89Sxx  1   // AT89S53/8252
#define DEVICE_LOCKED   2

extern TSignature Signatures[];
extern const int SIGNCOUNT;

int FindSignature(uint8_t s0, uint8_t s1, uint8_t s2);
int FindName(const char *name);

#endif // PROCESSORS_H
