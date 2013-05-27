#include <stdint.h>
#include <string.h>

#include "globals.h"
#include "processors.h"
#include "strcasecmp.h"

TSignature Signatures[] = {
  { 0, 0, 0, "AT89S53/8252", PROC_TYPE_OLD51, 12288, 2048, 0, 0, 0, 0, 0, 0, ALGO_STD, ALGO_ERASE_STD, ALGO_LB_89x, ALGO_BUSY_POLL_NMSB, 16,
    { "", "", "", "", "", "LB3", "LB2", "LB1" },
    { "", "", "", "", "", "", "", "" },
    { "", "", "", "", "", "", "", "" },
    { "", "", "", "", "", "", "", "" } },

  { 0, 0x1, 0x2, "CHIP LOCKED", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    { "", "", "", "", "", "", "", "" },
    { "", "", "", "", "", "", "", "" },
    { "", "", "", "", "", "", "", "" },
    { "", "", "", "", "", "", "", "" } },

#include "devices_at89.inc"
#include "devices_at90.inc"
#include "devices_attiny.inc"
#include "devices_atmega.inc"
#include "devices_at45.inc"
#include "devices_at25.inc"
#include "devices_at25f.inc"
#include "devices_at26.inc"

#ifdef I2C_SUPPORT
    #include "devices_at24.inc"
#endif
};

const int SIGNCOUNT = (int)(sizeof(Signatures) / sizeof(Signatures[0]));

int proc_find_signature(uint8_t s0, uint8_t s1, uint8_t s2)
{
	int n;
	for (n = 1; n < SIGNCOUNT; n++) {
		TSignature *item = &Signatures[n];
		if ((item->b0 == s0) && (item->b1 == s1) && (item->b2 == s2)) {
			return n;
		}
	}
	return -1;
}

int proc_find_name(const char *name)
{
	int n;
	for (n = 0; n < SIGNCOUNT; n++) {
		if ((n != DEVICE_LOCKED) && (mystrcasecmp(Signatures[n].name, name) == 0)) {
			return n;
		}
	}
	return -1;
}

// end of file
