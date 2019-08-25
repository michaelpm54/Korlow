#ifndef INST_DATA_H
#define INST_DATA_H

#include "types.h"

extern const int kInstSizes[0x100];
extern const int kInstCycles[0x100];
extern const int kInstCyclesAlt[0x100];

extern const char * const kInstFmts[0x100];
extern const int   kInstFmtSizes[0x100];


extern const int kCbInstSizes[0x100];
extern const int kCbInstCycles[0x100];

extern const char * const kCbInstFmts[0x100];
extern const int   kCbInstFmtSizes[0x100];

#endif // INST_DATA_H
