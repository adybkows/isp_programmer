#include <string.h>
#include <windows.h>

static const char MASTER_KEY[] = "Software\\ISPProg";

int get_reg(const char *n, const char *defval, char *buff, int buffsize)
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, MASTER_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		DWORD type, len = buffsize;
		if (RegQueryValueEx(hKey, n, NULL, &type, (LPBYTE) buff, &len) == ERROR_SUCCESS && type == REG_SZ) {
			RegCloseKey(hKey);
			return (int) len;
		}
		RegCloseKey(hKey);
	}
	strncpy(buff, defval, buffsize);
	buff[buffsize - 1] = 0;
	return strlen(defval) + 1;
}

int get_reg_int(const char *n, int defval)
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, MASTER_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		DWORD type, value, len = sizeof(DWORD);
		if (RegQueryValueEx(hKey, n, NULL, &type, (LPBYTE) &value, &len) == ERROR_SUCCESS && type == REG_DWORD) {
			RegCloseKey(hKey);
			return (int) value;
		}
		RegCloseKey(hKey);
	}
	return defval;
}

void set_reg(const char *n, const char *v)
{
	HKEY hKey;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, MASTER_KEY, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
		RegSetValueEx(hKey, n, 0, REG_SZ, (const BYTE *) v, strlen(v) + 1);
		RegCloseKey(hKey);
	}
}

void set_reg_int(const char *n, int v)
{
	HKEY hKey;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, MASTER_KEY, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
		DWORD value = (DWORD) v;
		RegSetValueEx(hKey, n, 0, REG_DWORD, (const BYTE *) &value, sizeof(DWORD));
		RegCloseKey(hKey);
	}
}

// end of file
