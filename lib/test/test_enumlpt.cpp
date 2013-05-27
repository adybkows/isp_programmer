#include <iostream>
#include <iomanip>

#include "ispprog.h"

using namespace std;

int main(void)
{
	TLPTinfo ports[16];

	int res = enum_lpt(ports, 16);
	cout << "enum_lpt res=" << res << endl;

	for (int i = 0; i < res; i++) {
		cout << "  LPT" << ports[i].num << " (0x" << setfill('0') << setw(4) << hex << ports[i].baseaddr << ")" << endl;
	}

	return (res >= 0) ? 0 : 1;
}
