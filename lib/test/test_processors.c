#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ispprog.h"

int main(int argc, char **argv)
{
	if (argc == 2) {
		const char *name = argv[1];
		int res = FindName(name);
		if (res >= 0) {
			printf("%d\n", res);
			return 0;
		} else {
			printf("%s not found.\n", name);
			return 1;
		}
	} else if (argc == 4) {
		uint8_t b0 = strtol(argv[1], NULL, 0);
		uint8_t b1 = strtol(argv[2], NULL, 0);
		uint8_t b2 = strtol(argv[3], NULL, 0);
		int res = FindSignature(b0, b1, b2);
		if (res >= 1) {
			printf("%s\n", Signatures[res].name);
			return 0;
		} else {
			printf("%02X,%02X,%02X not found.\n", b0, b1, b2);
			return 1;
		}
	} else {
		printf("test_processors name | sig1 sig2 sig3\n");
		return 2;
	}
}
