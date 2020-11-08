#ifndef FS_H
#define FS_H

#include <string>
#include <vector>

namespace FS {

std::string readText(const std::string &path);
std::vector<std::uint8_t> read_bytes(const std::string &path);
void write_bytes(const std::string &path, const uint8_t *bytes, int count);

}    // namespace FS

#endif    // FS_H
