#ifndef ISP_FLASH_H
#define ISP_FLASH_H

#include <stdint.h>

void wait_ready_flash(uint32_t address, uint8_t value);
void isp_load_extended_address(uint32_t address);
int isp_read_flash_make_request(uint32_t address, void *buf);
uint8_t isp_read_flash(uint32_t address);
void isp_read_flash_page(uint32_t address, void *buf);
void isp_write_flash(uint32_t address, uint8_t value);
void isp_write_flash_page(uint32_t address, void *buf);
void isp_write_program_page(uint32_t address, uint8_t value);

#endif // ISP_FLASH_H
