#include <stdbool.h>
#include <windows.h>

#include "os/windows/inpout32.h"

#ifdef __MINGW64__
  #define INPOUT_DLL "inpoutx64.dll"
#else
  #define INPOUT_DLL "inpout32.dll"
#endif

typedef void (__stdcall *lpOut32)(short, short);
typedef short (__stdcall *lpInp32)(short);
typedef BOOL (__stdcall *lpIsInpOutDriverOpen)(void);
typedef BOOL (__stdcall *lpIsXP64Bit)(void);

// Global function pointers
static lpOut32 gfpOut32 = NULL;
static lpInp32 gfpInp32 = NULL;
static lpIsInpOutDriverOpen gfpIsInpOutDriverOpen = NULL;
static lpIsXP64Bit gfpIsXP64Bit = NULL;

static HINSTANCE hInpOutDll = NULL;

// Driver initialization
bool InpOut32Init(void)
{
	if (hInpOutDll != NULL)
		return true;

	// Dynamically load the DLL at runtime (not linked at compile time)
	hInpOutDll = LoadLibrary(INPOUT_DLL);
	if (hInpOutDll == NULL)
		return false;

	gfpOut32 = (lpOut32) GetProcAddress(hInpOutDll, "Out32");
	gfpInp32 = (lpInp32) GetProcAddress(hInpOutDll, "Inp32");
	gfpIsInpOutDriverOpen = (lpIsInpOutDriverOpen) GetProcAddress(hInpOutDll, "IsInpOutDriverOpen");
	gfpIsXP64Bit = (lpIsXP64Bit) GetProcAddress(hInpOutDll, "IsXP64Bit");

	if (gfpOut32 == NULL || gfpInp32 == NULL || gfpIsInpOutDriverOpen == NULL || gfpIsXP64Bit == NULL || !gfpIsInpOutDriverOpen()) {
		FreeLibrary(hInpOutDll);
		gfpOut32 = NULL;
		gfpInp32 = NULL;
		gfpIsInpOutDriverOpen = NULL;
		gfpIsXP64Bit = NULL;
		hInpOutDll = NULL;
		return false;
	}

	return true;
}

// Driver shutdown
void InpOut32Shutdown(void)
{
	if (hInpOutDll != NULL) {
		FreeLibrary(hInpOutDll);
		gfpOut32 = NULL;
		gfpInp32 = NULL;
		gfpIsInpOutDriverOpen = NULL;
		gfpIsXP64Bit = NULL;
		hInpOutDll = NULL;
	}
}

// Write data
void Out32(uint16_t port_address, uint8_t data)
{
	if (gfpOut32 != NULL)
		gfpOut32(port_address, data);
}

// Read data
uint8_t Inp32(uint16_t port_address)
{
	return (gfpInp32 != NULL) ? gfpInp32(port_address) & 0xff : 0;
}

// Returns true if the InpOut driver was opened successfully
bool IsInpOutDriverOpen(void)
{
	return (gfpIsInpOutDriverOpen != NULL) ? gfpIsInpOutDriverOpen() : false;
}

// Returns true if the OS is 64bit (x64) Windows.
bool IsXP64Bit(void)
{
	return (gfpIsXP64Bit != NULL) ? gfpIsXP64Bit() : false;
}

// end of file
