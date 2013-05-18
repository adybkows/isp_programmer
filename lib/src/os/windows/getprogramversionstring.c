#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

char *GetProgramVersionString(char *buffer, int buffersize)
{
	char filename[MAX_PATH];
	DWORD zero;
	DWORD infolen;
	void *infobuf;
	UINT verlen;
	void *ptr;
	VS_FIXEDFILEINFO *verptr;

	if (GetModuleFileName(0, filename, sizeof(filename)) > 0) {
		filename[sizeof(filename) - 1] = 0;
		if (strlen(filename) > 0) {
			zero = 0;
			infolen = GetFileVersionInfoSize(filename, &zero);
			if (infolen > 0) {
				infobuf = malloc(infolen);
				if (infobuf) {
					ptr = NULL;
					verlen = 0;
					if (GetFileVersionInfo(filename, 0, infolen, infobuf)) {
						if (VerQueryValue(infobuf, "\\", &ptr, &verlen)) {
							verptr = (VS_FIXEDFILEINFO *) ptr;
							snprintf(buffer, buffersize, "%u.%u.%u.%u", (unsigned int) verptr->dwFileVersionMS >> 16,
								(unsigned int) verptr->dwFileVersionMS & 0xffff,
								(unsigned int) verptr->dwFileVersionLS >> 16,
								(unsigned int) verptr->dwFileVersionLS & 0xffff);
							free(infobuf);
							return buffer;
						}
					}
					free(infobuf);
				}
			}
		}
	}
	return NULL;
}
