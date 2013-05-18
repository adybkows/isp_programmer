#ifndef ENUMLPT_H
#define ENUMLPT_H

#include <stdint.h>

typedef struct
{
	int num;
	uint16_t baseaddr;
} TLPTinfo;

int EnumLPT(TLPTinfo *lpttab, int maxports);

#endif // ENUMLPT_H
