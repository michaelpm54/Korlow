#ifndef UTIL_H
#define UTIL_H

#include <GL/gl.h>
#include <memory>

std::string readFileText(const std::string &filePath, int &size);
std::unique_ptr<uint8_t[]> readFileBytes(const std::string &filePath, int &size);
void loadShaders(GLuint program, const std::string &vsPath, const std::string &fsPath);

#endif // UTIL_H
