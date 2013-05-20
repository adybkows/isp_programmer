#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "membuffer.h"
#include "processors.h"

static void syntax(void)
{
	printf("test_membuffer ishex|ishexfname|hex2bin|bin2hex infile [outfile]\n");
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		syntax();
		return 2;
	}

	const char *cmd = argv[1];
	const char *infilename = argv[2];
	const char *outfilename = (argc == 4) ? argv[3] : NULL;

	if (!strcmp(cmd, "ishex")) {
		bool res = IsIntelHex(infilename, BUF_FLASH);
		printf("%s\n", res ? "true" : "false");
		// OK if hex
		return !res;
	} else if (!strcmp(cmd, "ishexfname")) {
		bool res = IsIntelHexFilename(infilename);
		printf("%s\n", res ? "true" : "false");
		// OK if hex file name
		return !res;
	} else if (!strcmp(cmd, "hex2bin") && (argc == 4)) {
		uint32_t minaddr, maxaddr;
		const char *res = LoadIntelHex(BUF_FLASH, infilename, MAX_FLASH_SIZE, &minaddr, &maxaddr);
		if (*res) {
			printf("%s\n", res);
			return 2;
		}
		printf("minaddr=0x%x maxaddr=0x%x len=%u\n", minaddr, maxaddr, maxaddr + 1);
		res = SaveBinaryFile(BUF_FLASH, outfilename, maxaddr + 1);
		if (*res) {
			printf("%s\n", res);
			return 2;
		}
		return 0;
	} else if (!strcmp(cmd, "bin2hex") && (argc == 4)) {
		uint32_t minaddr, maxaddr;
		const char *res = LoadBinaryFile(BUF_FLASH, infilename, MAX_FLASH_SIZE, &minaddr, &maxaddr);
		if (*res) {
			printf("%s\n", res);
			return 2;
		}
		printf("minaddr=0x%x maxaddr=0x%x len=%u\n", minaddr, maxaddr, maxaddr + 1);
		res = SaveIntelHex(BUF_FLASH, outfilename, maxaddr + 1);
		if (*res) {
			printf("%s\n", res);
			return 2;
		}
		return 0;
	}

	syntax();
	return 2;
}
