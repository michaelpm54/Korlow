#ifndef FS_H
#define FS_H

#include <string>
#include <vector>

namespace FS
{

std::string readText(const std::string &path);
std::vector<std::uint8_t> readBytes(const std::string &path);

}

#endif // FS_H
