#ifndef FS_H
#define FS_H

#include <string>
#include <vector>

#include "emu_types.h"

namespace FS {

std::string readText(const std::string &path);
std::vector<u8> read_bytes(const std::string &path);
void write_bytes(const std::string &path, const u8 *bytes, int count);

}    // namespace FS

#endif    // FS_H
