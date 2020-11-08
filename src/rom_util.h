#ifndef ROMUTIL_H
#define ROMUTIL_H

#include <vector>

#include "emu_types.h"

void printRomInfo(const std::vector<u8> &rom);
std::vector<u8> ghostRom();

#endif    // ROMUTIL_H
