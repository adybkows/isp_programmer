#ifndef PINSIO_H
#define PINSIO_H

#include <stdbool.h>

void RstOn(void);
void RstOff(void);
void StrobeOn(void);
void StrobeOff(void);
void ChipselectOn(void);
void ChipselectOff(void);
bool ReadRst(void);
void ClkLo(void);
void ClkHi(void);
void Send0(void);
void Send1(void);
void LedOn(void);
void LedOff(void);
bool ReadBit(void);
void PinoutChanged(void);

#endif // PINSIO_H
