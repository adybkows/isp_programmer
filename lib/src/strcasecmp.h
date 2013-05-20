#ifndef STRCASECMP_H
#define STRCASECMP_H

#include <ctype.h>

static inline int mystrcasecmp(const char *s1, const char *s2)
{
	for (;;) {
		unsigned char c1 = (unsigned char) *s1++;
		unsigned char c2 = (unsigned char) *s2++;
		int d = tolower(c1) - tolower(c2);
		if ((d != 0) || (c1 == (unsigned char) 0))
			return d;
	}
}

#endif // STRCASECMP_H
