#ifndef ISP_USERSIGN_H
#define ISP_USERSIGN_H

#include <stdint.h>

int ISPReadUserSign_MakeRequest(uint32_t address, void *buf);
uint8_t ISPReadUserSign(uint32_t address);
void ISPWriteUserSign(uint32_t address, uint8_t value);

#endif // ISP_USERSIGN_H
