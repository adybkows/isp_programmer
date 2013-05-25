#ifndef ISP_LOCKBITS_H
#define ISP_LOCKBITS_H

#include <stdbool.h>
#include <stdint.h>

int ISPReadLockBits(uint8_t *lb);
int ISPWriteLockBits(uint8_t lb);
bool AnyLock(void);

#endif // ISP_LOCKBITS_H
