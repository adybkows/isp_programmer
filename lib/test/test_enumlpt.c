#include <stdio.h>

#include "enumlpt.h"

int main(int argc, char **argv)
{
	TLPTinfo ports[16];

	int res = EnumLPT(ports, 16);
	printf("EnumLPT res=%d\n", res);

	for (int i = 0; i < res; i++) {
		printf("  LPT%d (0x%04x)\n", ports[i].num, ports[i].baseaddr);
	}

	return (res >= 0) ? 0 : 1;
}
