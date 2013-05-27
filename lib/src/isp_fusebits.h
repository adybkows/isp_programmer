#ifndef ISP_FUSEBITS_H
#define ISP_FUSEBITS_H

#include <stdbool.h>
#include <stdint.h>

int isp_read_fusebits(uint8_t *fblo, uint8_t *fbhi, uint8_t *fbext);
void isp_write_low_fusebits(uint8_t fuse);
void isp_write_high_fusebits(uint8_t fuse);
void isp_write_ext_fusebits(uint8_t fuse);
bool any_fusebits_low(void);
bool any_fusebits_high(void);
bool any_fusebits_ext(void);
bool any_fusebits(void);

#endif // ISP_FUSEBITS_H
