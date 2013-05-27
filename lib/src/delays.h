#ifndef DELAYS_H
#define DELAYS_H

#include <stdint.h>

void delays_init(void);

void wait_s(int x);
void wait_ms(int x);
void wait_us(int x);
void wait_ns(int x);

void tic(int64_t *t);
int64_t toc_ms(int64_t t);
int64_t toc_us(int64_t t);
int64_t toc_ns(int64_t t);

#endif // DELAYS_H
