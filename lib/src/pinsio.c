#include "globals.h"
#include "portsio.h"

static uint8_t last_data, last_control;

static void set_data_bit(uint8_t nr)
{
	last_data |= (1 << nr);
	out_port(BASE, last_data);
}

static void clear_data_bit(uint8_t nr)
{
	last_data &= ~(1 << nr);
	out_port(BASE, last_data);
}

static bool get_data_bit(uint8_t nr)
{
	return ((last_data & (1 << nr)) != 0);
}

static void set_control_bit(uint8_t nr)
{
	last_control |= (1 << nr);
	out_port(BASE + ControlPort, last_control);
}

static void clear_control_bit(uint8_t nr)
{
	last_control &= ~(1 << nr);
	out_port(BASE + ControlPort, last_control);
}

static bool get_control_bit(uint8_t nr)
{
	return ((last_control & (1 << nr)) != 0);
}

static bool get_status_bit(uint8_t nr)
{
	return ((in_port(BASE + StatusPort) & (1 << nr)) != 0);
}

static void set_lpt_bit(uint8_t nr)
{
	switch (nr) {
		case LPT_OUT_STROBE: /* inverted */
			clear_control_bit(0);
			break;

		case LPT_OUT_D0:
			set_data_bit(0);
			break;

		case LPT_OUT_D1:
			set_data_bit(1);
			break;

		case LPT_OUT_D2:
			set_data_bit(2);
			break;

		case LPT_OUT_D3:
			set_data_bit(3);
			break;

		case LPT_OUT_D4:
			set_data_bit(4);
			break;

		case LPT_OUT_D5:
			set_data_bit(5);
			break;

		case LPT_OUT_D6:
			set_data_bit(6);
			break;

		case LPT_OUT_D7:
			set_data_bit(7);
			break;

		case LPT_OUT_AUTOLF: /* inverted */
			clear_control_bit(1);
			break;

		case LPT_OUT_INIT:
			set_control_bit(2);
			break;

		case LPT_OUT_SELECTIN: /* inverted */
			clear_control_bit(3);
			break;
	}
}

static void clear_lpt_bit(uint8_t nr)
{
	switch (nr) {
		case LPT_OUT_STROBE: /* inverted */
			set_control_bit(0);
			break;

		case LPT_OUT_D0:
			clear_data_bit(0);
			break;

		case LPT_OUT_D1:
			clear_data_bit(1);
			break;

		case LPT_OUT_D2:
			clear_data_bit(2);
			break;

		case LPT_OUT_D3:
			clear_data_bit(3);
			break;

		case LPT_OUT_D4:
			clear_data_bit(4);
			break;

		case LPT_OUT_D5:
			clear_data_bit(5);
			break;

		case LPT_OUT_D6:
			clear_data_bit(6);
			break;

		case LPT_OUT_D7:
			clear_data_bit(7);
			break;

		case LPT_OUT_AUTOLF: /* inverted */
			set_control_bit(1);
			break;

		case LPT_OUT_INIT:
			clear_control_bit(2);
			break;

		case LPT_OUT_SELECTIN: /* inverted */
			set_control_bit(3);
			break;
	}
}

static bool get_lpt_bit(uint8_t nr)
{
	switch (nr) {
		case LPT_IN_ACK:
			return get_status_bit(6);

		case LPT_IN_BUSY: /* inverted */
			return !get_status_bit(7);

		case LPT_IN_PAPEREND:
			return get_status_bit(5);

		case LPT_IN_SELECT:
			return get_status_bit(4);

		case LPT_IN_ERROR:
			return get_status_bit(3);

		default:
			return false;
	}
}

void reset_on()
{
	bool v;

	v = (proctype == PROC_TYPE_AVR) ^ (!pinout.resetinv);
	if (v)
		set_lpt_bit(pinout.reset);
	else
		clear_lpt_bit(pinout.reset);
}

void reset_off()
{
	bool v;

	v = (proctype == PROC_TYPE_AVR) ^ pinout.resetinv;
	if (v)
		set_lpt_bit(pinout.reset);
	else
		clear_lpt_bit(pinout.reset);
}

void strobe_on()
{
	clear_lpt_bit(pinout.strobe1);
	clear_lpt_bit(pinout.strobe2);
}

void strobe_off()
{
	set_lpt_bit(pinout.strobe1);
	set_lpt_bit(pinout.strobe2);
}

void chipselect_on()
{
	clear_lpt_bit(pinout.reset);
}

void chipselect_off()
{
	set_lpt_bit(pinout.reset);
}

bool read_reset()
{
	switch (pinout.reset) {
		case LPT_OUT_STROBE: /* inverted */
			return !get_control_bit(0);

		case LPT_OUT_D0:
			return get_data_bit(0);

		case LPT_OUT_D1:
			return get_data_bit(1);

		case LPT_OUT_D2:
			return get_data_bit(2);

		case LPT_OUT_D3:
			return get_data_bit(3);

		case LPT_OUT_D4:
			return get_data_bit(4);

		case LPT_OUT_D5:
			return get_data_bit(5);

		case LPT_OUT_D6:
			return get_data_bit(6);

		case LPT_OUT_D7:
			return get_data_bit(7);

		case LPT_OUT_AUTOLF: /* inverted */
			return !get_control_bit(1);

		case LPT_OUT_INIT:
			return get_control_bit(2);

		case LPT_OUT_SELECTIN: /* inverted */
			return !get_control_bit(3);

		default:
			return false;
	}
}

void clk_lo()
{
	clear_lpt_bit(pinout.sck);
}

void clk_hi()
{
	set_lpt_bit(pinout.sck);
}

void send_0()
{
	clear_lpt_bit(pinout.mosi);
}

void send_1()
{
	set_lpt_bit(pinout.mosi);
}

void led_on()
{
	clear_lpt_bit(pinout.led);
}

void led_off()
{
	set_lpt_bit(pinout.led);
}

bool read_bit()
{
  return get_lpt_bit(pinout.miso);
}

void pinout_changed()
{
	last_data = in_port(BASE);
	last_control = in_port(BASE + ControlPort);
}

// end of file
