#ifndef ROMUTIL_H
#define ROMUTIL_H

#include <cstdint>
#include <vector>

void printRomInfo(const std::vector<std::uint8_t> &rom);
std::vector<std::uint8_t> ghostRom();

#endif // ROMUTIL_H
