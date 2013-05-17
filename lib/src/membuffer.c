#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "membuffer.h"
#include "processors.h"

uint8_t flashbuffer[MAX_FLASH_SIZE];
uint8_t eeprombuffer[MAX_EEPROM_SIZE];
uint8_t usersigbuffer[MAX_USERSIG_SIZE];

static char resultstring[1024];

void ClearBuffer(int bufid)
{
	switch (bufid) {
		case BUF_FLASH:
			memset(flashbuffer, 0xff, sizeof(flashbuffer));
			break;
		case BUF_EEPROM:
			memset(eeprombuffer, 0xff, sizeof(eeprombuffer));
			break;
		case BUF_USERSIG:
			memset(usersigbuffer, 0xff, sizeof(usersigbuffer));
			break;
	}
}

const char *LoadFile(int bufid, int filetype, const char *fname, uint32_t buflen, uint32_t *minaddr, uint32_t *maxaddr)
{
	if (filetype == FILE_TYPE_INTELHEX)
		return LoadIntelHex(bufid, fname, buflen, minaddr, maxaddr);
	else
		return LoadBinaryFile(bufid, fname, buflen, minaddr, maxaddr);
}

const char *SaveFile(int bufid, int filetype, const char *fname, uint32_t buflen)
{
	if (filetype == FILE_TYPE_INTELHEX)
		return SaveIntelHex(bufid, fname, buflen);
	else
		return SaveBinaryFile(bufid, fname, buflen);
}

static uint8_t hex2byte(const char *c, int *error)
{
	*error = -1;

	if (!isxdigit((int) c[0]) || !isxdigit((int) c[1]))
		return 0;

	char s[] = { c[0], c[1], '\0' };
	char *ptr = NULL;
	long v = strtol(s, &ptr, 16);

	if (v == LONG_MAX || v == LONG_MIN || ptr != c + 2)
		return 0;

	*error = 0;
	return (uint8_t) v;
}

static uint16_t hex2short(const char *c, int *error)
{
	uint16_t msb = hex2byte(c, error);
	if (*error)
		return 0;
	uint16_t lsb = hex2byte(c + 2, error);
	if (*error)
		return 0;
	return (msb << 8) | lsb;
}

const char *LoadIntelHex(int bufid, const char *fname, uint32_t buflen, uint32_t *minaddr, uint32_t *maxaddr)
{
	char line[1024];
	char *p_cr, *p_lf;
	int aktline = 0, code;
	uint32_t segadr = 0, radr;
	uint8_t b, rlen, rtyp = 1, x, n;
	FILE *f;

	if (buflen == 0)
		return "Can't load file";

	f = fopen(fname, "r");
	if (f == NULL) {
		snprintf(resultstring, sizeof(resultstring), "Can't open file %s", fname);
		return resultstring;
	}
	*minaddr = INT_MAX;
	*maxaddr = INT_MIN;
	do {
		if (feof(f) || (fgets(line, sizeof(line), f) == NULL))
			break;

		do {
			aktline++;
			if (line[0]) {
				if (line[1] != ':')
					goto bailout;
				rlen = hex2byte(line + 2, &code);
				if (code != 0)
					goto bailout;
				radr = hex2short(line + 4, &code);
				if (code != 0)
					goto bailout;
				rtyp = hex2byte(line + 8, &code);
				if (code != 0)
					goto bailout;
				if (rtyp == 2) {
					// Extended Segment Address Record
					b = hex2byte(line + 10, &code);
					if (code != 0)
						goto bailout;
					segadr = ((uint32_t) b) << 12;
					b = hex2byte(line + 12, &code);
					if (code != 0)
						goto bailout;
					segadr += (((uint32_t) b) << 4);
				} else if (rtyp == 4) {
					// Extended Linear Address Record
					b = hex2byte(line + 10, &code);
					if (code != 0)
						goto bailout;
					segadr = ((uint32_t) b) << 24;
					b = hex2byte(line + 12, &code);
					if (code != 0)
						goto bailout;
					segadr += (((uint32_t) b) << 16);
				} else if (rtyp == 0) {
					// Data Record
					radr += segadr; // add Extended Segment Address
					x = 10;
					for (n = 0; n < rlen; n++) {
						b = hex2byte(line + x, &code);
						if (code != 0)
							goto bailout;
						if (radr < buflen) {
							if (bufid == BUF_FLASH)
								flashbuffer[radr] = b;
							else if (bufid == BUF_EEPROM)
								eeprombuffer[radr] = b;
							else if (bufid == BUF_USERSIG)
								usersigbuffer[radr] = b;
							// search lowest used address
							if (*minaddr > radr)
								*minaddr = radr;
							// search highest used address
							if (*maxaddr < radr)
								*maxaddr = radr;
						}
						x += 2;
						radr++;
					}
				}
			}
			p_cr = strchr(line, '\x0d');
			if (p_cr)
				memmove(line, p_cr + 1, strlen(p_cr));
			p_lf = strchr(line, '\x0a');
			if (p_lf)
				memmove(line, p_lf + 1, strlen(p_lf));
		} while (line[0] && (p_cr || p_lf));
	} while (rtyp != 1 && !feof(f));

	fclose(f);
	return "";

bailout:
	fclose(f);
	snprintf(resultstring, sizeof(resultstring), "Error in file %s, line %d", fname, aktline);
	return resultstring;
}

const char *SaveIntelHex(int bufid, const char *fname, uint32_t buflen)
{
	char line[64];
	FILE *f;
	uint32_t radr, d1;
	uint8_t b, n, rsum, rlen;
	bool use_segaddr, use_linaddr;

	if (buflen == 0)
		return "Can't save file";

	f = fopen(fname, "w");
	if (f == NULL) {
		snprintf(resultstring, sizeof(resultstring), "Can't create file %s", fname);
		return resultstring;
	}

	use_segaddr = (buflen > 65536) && (buflen <= 1048576);
	use_linaddr = (buflen > 1048576);

	radr = 0;
	d1 = buflen;

	do {
		if ((radr > 0) && ((radr & 0xffff) == 0)) {
			if (use_segaddr) {
				// Extendedd Segment Address Record (data size over 64 KB and up to 1 MB)
				rsum = 4 + ((radr >> 12) & 0xff) + ((radr >> 4) & 0xff);
				fprintf(f, ":02000002%04x%02x\n", radr >> 4, (uint8_t)(0 - rsum));
			} else if (use_linaddr) {
				// Extended Linear Address Record (data size over 1 MB)
				rsum = 6 + ((radr >> 24) & 0xff) + ((radr >> 16) & 0xff);
				fprintf(f, ":02000004%04x%02x\n", radr >> 16, (uint8_t)(0 - rsum));
			}
		}
		if (d1 > 16)
			rlen = 16;
		else
			rlen = d1;
		sprintf(line, ":%02x%04x00", rlen, radr & 0xffff);
		rsum = rlen + (uint8_t)(radr >> 8) + (uint8_t)(radr & 0xff);
		for (n = 0; n < rlen; n++) {
			if (bufid == BUF_FLASH)
				b = flashbuffer[radr];
			else if (bufid == BUF_EEPROM)
				b = eeprombuffer[radr];
			else if (bufid == BUF_USERSIG)
				b = usersigbuffer[radr];
			else
				b = 0xff;
			sprintf(line + strlen(line), "%02x", b);
			rsum = rsum + b;
			radr++;
			d1--;
		}
		fprintf(f, "%s%02x\n", line, (uint8_t)(0 - rsum));
	} while (d1 > 0);
	fprintf(f, ":00000001FF\n");
	fclose(f);
	return "";
}

const char *LoadBinaryFile(int bufid, const char *fname, uint32_t buflen, uint32_t *minaddr, uint32_t *maxaddr)
{
	FILE *f;
	size_t numrd = 0;

	if (buflen == 0)
		return "Can't load file";

	f = fopen(fname, "rb");
	if (f == NULL) {
		snprintf(resultstring, sizeof(resultstring), "Can't open file %s", fname);
		return resultstring;
	}

	*minaddr = 0;

	if (bufid == BUF_FLASH)
		numrd = fread(flashbuffer, 1, buflen, f);
	else if (bufid == BUF_EEPROM)
		numrd = fread(eeprombuffer, 1, buflen, f);
	else if (bufid == BUF_USERSIG)
		numrd = fread(usersigbuffer, 1, buflen, f);

	fclose(f);

	if (numrd <= 0) {
		snprintf(resultstring, sizeof(resultstring), "File %s is empty", fname);
		return resultstring;
	}

	*maxaddr = numrd - 1;
	return "";
}

#if 0

function SaveBinaryFile(uint8_t bufid; fname:string; buflen:int):string;
var
  f:file;
const
  numwr:int == 0;
{
  if buflen == 0 {
  {
    Result = 'Can''t save file';
    Exit;
  }
  AssignFile(f, UTF8ToSys(fname));
  {$I-} Rewrite(f, 1); {$I+}
  if IOResult != 0 {
  {
    Result = 'Can''t create file ' + fname;
    Exit;
  }
  if bufid == BUF_FLASH {
    BlockWrite(f, flashbuffer, buflen, numwr)
  else if bufid == BUF_EEPROM {
    BlockWrite(f, eeprombuffer, buflen, numwr)
  else if bufid == BUF_USERSIG {
    BlockWrite(f, usersigbuffer, buflen, numwr);
  CloseFile(f);
  if numwr != buflen {
  {
    Result = 'Can''t write file ' + fname;
    Exit;
  }
  Result = '';
}

function HighestUsed(uint8_t bufid; buflen:int):int;
var i:int;
{
  if bufid == BUF_FLASH {
  {
    for i = buflen - 1 downto 0 do
      if flashbuffer[i] != 0xff {
      {
        Result = i;
        Exit;
      }
      Result = -1;
  }
  else if bufid == BUF_EEPROM {
  {
    for i = buflen - 1 downto 0 do
      if eeprombuffer[i] != 0xff {
      {
        Result = i;
        Exit;
      }
      Result = -1;
  }
  else if bufid == BUF_USERSIG {
  {
    for i = buflen - 1 downto 0 do
      if usersigbuffer[i] != 0xff {
      {
        Result = i;
        Exit;
      }
      Result = -1;
  }
  else
    Result = -1;
}

function LowestUsed(uint8_t bufid; buflen:int):int;
var i:int;
{
  if bufid == BUF_FLASH {
  {
    for i = 0 to buflen - 1 do
      if flashbuffer[i] != 0xff {
      {
        Result = i;
        Exit;
      }
      Result = -1;
  }
  else if bufid == BUF_EEPROM {
  {
    for i = 0 to buflen - 1 do
      if eeprombuffer[i] != 0xff {
      {
        Result = i;
        Exit;
      }
      Result = -1;
  }
  else if bufid == BUF_USERSIG {
  {
    for i = 0 to buflen - 1 do
      if usersigbuffer[i] != 0xff {
      {
        Result = i;
        Exit;
      }
      Result = -1;
  }
  else
    Result = -1;
}

function IsBlockEmpty(uint8_t bufid; startadr:int; len:int):boolean;
var i:int;
{
  Result = false;
  if bufid == BUF_FLASH {
  {
    for i = startadr to startadr + len - 1 do
      if flashbuffer[i] != 0xff {
        Exit;
  }
  else if bufid == BUF_EEPROM {
  {
    for i = startadr to startadr + len - 1 do
      if eeprombuffer[i] != 0xff {
        Exit;
  }
  else if bufid == BUF_USERSIG {
  {
    for i = startadr to startadr + len - 1 do
      if usersigbuffer[i] != 0xff {
        Exit;
  }
  Result = true;
}

function IsIntelHex(fname:string; uint8_t uint8_tbufid):boolean;
var buflen, minaddr, maxaddr:int;
{
  Result = false;
  if bufid == BUF_FLASH {
    buflen = MAX_FLASH_SIZE
  else if bufid == BUF_EEPROM {
    buflen = MAX_EEPROM_SIZE
  else if bufid == BUF_USERSIG {
    buflen = MAX_USERSIG_SIZE
  else
    Exit;
  maxaddr = 0; minaddr = 0;
  if LoadIntelHex(bufid, fname, buflen, minaddr, maxaddr) == '' {
    Result = true;
}

function IsIntelHexFilename(fname:string):boolean;
var ext:string;
{
  ext = LowerCase(ExtractFileExt(fname));
  Result = ((ext == 'hex') or (ext == 'eep') or (ext == 'rom') or (ext == 'ihx'));
}

#endif
