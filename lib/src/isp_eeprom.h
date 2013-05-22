#ifndef ISP_EEPROM_H
#define ISP_EEPROM_H

#include <stdint.h>

void WaitForReadyEEPROM(uint32_t address, uint8_t value);
int ISPReadEEPROM_MakeRequest(uint32_t address, void *buf);
uint8_t ISPReadEEPROM(uint32_t address);
void ISPReadEEPROMPage(uint32_t address, void *buf);
void ISPWriteEEPROM(uint32_t address, uint8_t value);
void ISPWriteEEPROMPage(uint32_t address, const void *buf);

#endif // ISP_EEPROM_H
