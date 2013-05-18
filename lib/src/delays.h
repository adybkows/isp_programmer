#ifndef DELAYS_H
#define DELAYS_H

#include <stdint.h>

void DelaysInit(void);

void WaitS(int x);
void WaitMS(int x);
void WaitUS(int x);
void WaitNS(int x);

void Tic(int64_t *t);
int64_t TocMS(int64_t t);
int64_t TocUS(int64_t t);
int64_t TocNS(int64_t t);

#endif // DELAYS_H
