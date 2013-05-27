#ifndef PINSIO_H
#define PINSIO_H

#include <stdbool.h>

void reset_on(void);
void reset_off(void);
void strobe_on(void);
void strobe_off(void);
void chipselect_on(void);
void chipselect_off(void);
bool read_reset(void);
void clk_lo(void);
void clk_hi(void);
void send_0(void);
void send_1(void);
void led_on(void);
void led_off(void);
bool read_bit(void);
void pinout_changed(void);

#endif // PINSIO_H
