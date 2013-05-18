#include <stdbool.h>
#include <stdint.h>

#include <windows.h>
#include <cfgmgr32.h>
#include <setupapi.h>

#include "enumlpt.h"

static const GUID lptcom = { 0x4D36E978, 0xE325, 0x11CE, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 } };

static bool GetLPTAddr(DEVINST dev, uint16_t *baseaddr)
{
	LOG_CONF conf;
	DWORD res = CM_Get_First_Log_Conf(&conf, dev, ALLOC_LOG_CONF);
	if (res != CR_SUCCESS || conf == 0)
		return false;

	RES_DES rsrc;
	res = CM_Get_Next_Res_Des(&rsrc, conf, ResType_IO, NULL, 0);
	if (res != CR_SUCCESS) {
		CM_Free_Log_Conf_Handle(conf);
		return false;
	}

	char buf[256];
	res = CM_Get_Res_Des_Data(rsrc, buf, sizeof(buf), 0);
	if (res != CR_SUCCESS) {
		CM_Free_Log_Conf_Handle(conf);
		return false;
	}

	*baseaddr = ((PIO_RESOURCE) buf)->IO_Header.IOD_Alloc_Base & 0xffff;
	CM_Free_Log_Conf_Handle(conf);
	return true;
}

int EnumLPT(TLPTinfo *lpttab, int maxports)
{
	HDEVINFO h = SetupDiGetClassDevs(&lptcom, NULL, 0, DIGCF_PRESENT | DIGCF_PROFILE);
	if (h == INVALID_HANDLE_VALUE)
		return -1;

	int lptcnt = 0;
	int i = 0;
	BOOL res;

	do {
		SP_DEVINFO_DATA devinfo_data;
		devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
		res = SetupDiEnumDeviceInfo(h, i, &devinfo_data);
		if (res) {
			char dev_name[256];
			if (SetupDiGetDeviceRegistryProperty(h, &devinfo_data, SPDRP_FRIENDLYNAME, NULL, (PBYTE) dev_name,
					sizeof(dev_name), NULL)) {
				char *p1 = strstr(dev_name, "(LPT");
				if (p1 != NULL) {
					char *p2 = strchr(p1, ')');
					if ((p2 != NULL) && (p2 > p1) && (p2 - p1 - 5 <= 1)) {
						*p2 = '\0';
						int lptnum = atoi(p1 + 4);
						if ((lptnum >= 1) && (lptnum <= 255)) {
							lpttab->num = lptnum;
							if (GetLPTAddr(devinfo_data.DevInst, &lpttab->baseaddr)) {
								lpttab++;
								lptcnt++;
							}
						}
					}
				}
			}
		}
		i++;
	} while (res && (i < maxports));

	SetupDiDestroyDeviceInfoList(h);
	return lptcnt;
}

// end of file
