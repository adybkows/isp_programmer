#ifndef ISP_FUSEBITS_H
#define ISP_FUSEBITS_H

#include <stdbool.h>
#include <stdint.h>

int ISPReadFuseBits(uint8_t *fblo, uint8_t *fbhi, uint8_t *fbext);
void ISPWriteFuseLoBits(uint8_t fuse);
void ISPWriteFuseHiBits(uint8_t fuse);
void ISPWriteFuseExtBits(uint8_t fuse);
bool AnyFuseLo(void);
bool AnyFuseHi(void);
bool AnyFuseExt(void);
bool AnyFuse(void);

#endif // ISP_FUSEBITS_H
