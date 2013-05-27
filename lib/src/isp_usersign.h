#ifndef ISP_USERSIGN_H
#define ISP_USERSIGN_H

#include <stdint.h>

int isp_read_user_signature_make_request(uint32_t address, void *buf);
uint8_t isp_read_user_signature(uint32_t address);
void isp_write_user_signature(uint32_t address, uint8_t value);

#endif // ISP_USERSIGN_H
