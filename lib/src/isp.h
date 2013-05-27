#ifndef ISP_H
#define ISP_H

#include <stdbool.h>
#include <stdint.h>

/* Error codes */

#define ERROR_STOP      (-1)
#define ERROR_TIMEOUT   (-2)
#define ERROR_PROGRAM   (-3)

void isp_enable(void);
bool isp_poll_ready(void);
int isp_erase(void);
void isp_read_signature(void *s);
void isp_read_osc_cal_bytes(uint8_t *b0, uint8_t *b1, uint8_t *b2, uint8_t *b3);
void isp_read_memory_block(int memtype, uint32_t address, void *buf, uint32_t len);

#endif // ISP_H
