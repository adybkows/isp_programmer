#include "globals.h"
#include "portsio.h"

static uint8_t last_data, last_control;

static void SetDataBit(uint8_t nr)
{
	last_data |= (1 << nr);
	OutPort(BASE, last_data);
}

static void ClearDataBit(uint8_t nr)
{
	last_data &= ~(1 << nr);
	OutPort(BASE, last_data);
}

static bool GetDataBit(uint8_t nr)
{
	return ((last_data & (1 << nr)) != 0);
}

static void SetControlBit(uint8_t nr)
{
	last_control |= (1 << nr);
	OutPort(BASE + ControlPort, last_control);
}

static void ClearControlBit(uint8_t nr)
{
	last_control &= ~(1 << nr);
	OutPort(BASE + ControlPort, last_control);
}

static bool GetControlBit(uint8_t nr)
{
	return ((last_control & (1 << nr)) != 0);
}

static bool GetStatusBit(uint8_t nr)
{
	return ((InPort(BASE + StatusPort) & (1 << nr)) != 0);
}

static void SetLPTBit(uint8_t nr)
{
	switch (nr) {
		case LPT_OUT_STROBE: /* inverted */
			ClearControlBit(0);
			break;

		case LPT_OUT_D0:
			SetDataBit(0);
			break;

		case LPT_OUT_D1:
			SetDataBit(1);
			break;

		case LPT_OUT_D2:
			SetDataBit(2);
			break;

		case LPT_OUT_D3:
			SetDataBit(3);
			break;

		case LPT_OUT_D4:
			SetDataBit(4);
			break;

		case LPT_OUT_D5:
			SetDataBit(5);
			break;

		case LPT_OUT_D6:
			SetDataBit(6);
			break;

		case LPT_OUT_D7:
			SetDataBit(7);
			break;

		case LPT_OUT_AUTOLF: /* inverted */
			ClearControlBit(1);
			break;

		case LPT_OUT_INIT:
			SetControlBit(2);
			break;

		case LPT_OUT_SELECTIN: /* inverted */
			ClearControlBit(3);
			break;
	}
}

static void ClearLPTBit(uint8_t nr)
{
	switch (nr) {
		case LPT_OUT_STROBE: /* inverted */
			SetControlBit(0);
			break;

		case LPT_OUT_D0:
			ClearDataBit(0);
			break;

		case LPT_OUT_D1:
			ClearDataBit(1);
			break;

		case LPT_OUT_D2:
			ClearDataBit(2);
			break;

		case LPT_OUT_D3:
			ClearDataBit(3);
			break;

		case LPT_OUT_D4:
			ClearDataBit(4);
			break;

		case LPT_OUT_D5:
			ClearDataBit(5);
			break;

		case LPT_OUT_D6:
			ClearDataBit(6);
			break;

		case LPT_OUT_D7:
			ClearDataBit(7);
			break;

		case LPT_OUT_AUTOLF: /* inverted */
			SetControlBit(1);
			break;

		case LPT_OUT_INIT:
			ClearControlBit(2);
			break;

		case LPT_OUT_SELECTIN: /* inverted */
			SetControlBit(3);
			break;
	}
}

static bool GetLPTBit(uint8_t nr)
{
	switch (nr) {
		case LPT_IN_ACK:
			return GetStatusBit(6);

		case LPT_IN_BUSY: /* inverted */
			return !GetStatusBit(7);

		case LPT_IN_PAPEREND:
			return GetStatusBit(5);

		case LPT_IN_SELECT:
			return GetStatusBit(4);

		case LPT_IN_ERROR:
			return GetStatusBit(3);

		default:
			return false;
	}
}

void RstOn()
{
	bool v;

	v = (proctype == PROC_TYPE_AVR) ^ (!pinout.resetinv);
	if (v)
		SetLPTBit(pinout.reset);
	else
		ClearLPTBit(pinout.reset);
}

void RstOff()
{
	bool v;

	v = (proctype == PROC_TYPE_AVR) ^ pinout.resetinv;
	if (v)
		SetLPTBit(pinout.reset);
	else
		ClearLPTBit(pinout.reset);
}

void StrobeOn()
{
	ClearLPTBit(pinout.strobe1);
	ClearLPTBit(pinout.strobe2);
}

void StrobeOff()
{
	SetLPTBit(pinout.strobe1);
	SetLPTBit(pinout.strobe2);
}

void ChipselectOn()
{
	ClearLPTBit(pinout.reset);
}

void ChipselectOff()
{
	SetLPTBit(pinout.reset);
}

bool ReadRst()
{
	switch (pinout.reset) {
		case LPT_OUT_STROBE: /* inverted */
			return !GetControlBit(0);

		case LPT_OUT_D0:
			return GetDataBit(0);

		case LPT_OUT_D1:
			return GetDataBit(1);

		case LPT_OUT_D2:
			return GetDataBit(2);

		case LPT_OUT_D3:
			return GetDataBit(3);

		case LPT_OUT_D4:
			return GetDataBit(4);

		case LPT_OUT_D5:
			return GetDataBit(5);

		case LPT_OUT_D6:
			return GetDataBit(6);

		case LPT_OUT_D7:
			return GetDataBit(7);

		case LPT_OUT_AUTOLF: /* inverted */
			return !GetControlBit(1);

		case LPT_OUT_INIT:
			return GetControlBit(2);

		case LPT_OUT_SELECTIN: /* inverted */
			return !GetControlBit(3);

		default:
			return false;
	}
}

void ClkLo()
{
	ClearLPTBit(pinout.sck);
}

void ClkHi()
{
	SetLPTBit(pinout.sck);
}

void Send0()
{
	ClearLPTBit(pinout.mosi);
}

void Send1()
{
	SetLPTBit(pinout.mosi);
}

void LedOn()
{
	ClearLPTBit(pinout.led);
}

void LedOff()
{
	SetLPTBit(pinout.led);
}

bool ReadBit()
{
  return GetLPTBit(pinout.miso);
}

void PinoutChanged()
{
	last_data = InPort(BASE);
	last_control = InPort(BASE + ControlPort);
}

// end of file
