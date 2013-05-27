#ifndef ISP_LOCKBITS_H
#define ISP_LOCKBITS_H

#include <stdbool.h>
#include <stdint.h>

int isp_read_lockbits(uint8_t *lb);
int isp_write_lockbits(uint8_t lb);
bool any_lockbits(void);

#endif // ISP_LOCKBITS_H
