#ifndef ISP_EEPROM_H
#define ISP_EEPROM_H

#include <stdint.h>

void wait_ready_eeprom(uint32_t address, uint8_t value);
int isp_read_eeprom_make_request(uint32_t address, void *buf);
uint8_t isp_read_eeprom(uint32_t address);
void isp_read_eeprom_page(uint32_t address, void *buf);
void isp_write_eeprom(uint32_t address, uint8_t value);
void isp_write_eeprom_page(uint32_t address, const void *buf);

#endif // ISP_EEPROM_H
