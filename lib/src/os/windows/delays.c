#include <stdint.h>
#include <windows.h>

static int64_t persec = 0;

void delays_init(void)
{
	LARGE_INTEGER t;

	t.QuadPart = 0;
	QueryPerformanceFrequency(&t);
	persec = t.QuadPart;
}

void wait_s(int x)
{
	Sleep(x * 1000);
}

void wait_ms(int x)
{
	Sleep(x);
}

void wait_us(int x)
{
	LARGE_INTEGER f1, f2;
	long c;

	QueryPerformanceCounter(&f1);
	c = f1.QuadPart + persec * x / 1000000L;
	if (c == f1.QuadPart)
		c = f1.QuadPart + 1;
	do {
		QueryPerformanceCounter(&f2);
	} while (f2.QuadPart < c);
}

void wait_ns(int x)
{
	LARGE_INTEGER f1, f2;
	long c;

	QueryPerformanceCounter(&f1);
	c = f1.QuadPart + persec * x / 1000000000L;
	if (c == f1.QuadPart)
		c = f1.QuadPart + 1;
	do {
		QueryPerformanceCounter(&f2);
	} while (f2.QuadPart < c);
}

void tic(int64_t *t)
{
	LARGE_INTEGER t1;

	QueryPerformanceCounter(&t1);
	*t = t1.QuadPart;
}

/* returns time difference in ms */
int64_t toc_ms(int64_t t)
{
  LARGE_INTEGER t2;

  QueryPerformanceCounter(&t2);
  return (t2.QuadPart - t) * 1000 / persec;
}

/* returns time difference in us */
int64_t toc_us(int64_t t)
{
  LARGE_INTEGER t2;

  QueryPerformanceCounter(&t2);
  return (t2.QuadPart - t) * 1000000L / persec;
}

/* returns time difference in ns */
int64_t toc_ns(int64_t t)
{
  LARGE_INTEGER t2;

  QueryPerformanceCounter(&t2);
  return (t2.QuadPart - t) * 1000000000L / persec;
}

// end of file
