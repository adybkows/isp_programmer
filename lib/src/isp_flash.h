#ifndef ISP_FLASH_H
#define ISP_FLASH_H

#include <stdint.h>

void WaitForReadyFlash(uint32_t address, uint8_t value);
void ISPLoadExtendedAddress(uint32_t address);
int ISPReadFlash_MakeRequest(uint32_t address, void *buf);
uint8_t ISPReadFlash(uint32_t address);
void ISPReadFlashPage(uint32_t address, void *buf);
void ISPWriteFlash(uint32_t address, uint8_t value);
void ISPWriteFlashPage(uint32_t address, void *buf);
void ISPWriteProgramPage(uint32_t address, uint8_t value);

#endif // ISP_FLASH_H
