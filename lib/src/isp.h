#ifndef ISP_H
#define ISP_H

#include <stdbool.h>
#include <stdint.h>

/* Error codes */

#define ERROR_STOP      (-1)
#define ERROR_TIMEOUT   (-2)
#define ERROR_PROGRAM   (-3)

void ISPEnable(void);
bool ISPPollReady(void);
int ISPErase(void);
void ISPReadSign(void *s);
void ISPReadOscCalBytes(uint8_t *b0, uint8_t *b1, uint8_t *b2, uint8_t *b3);
void ISPReadMemoryBlock(int memtype, uint32_t address, void *buf, uint32_t len);

#endif // ISP_H
