#ifndef UTIL_H
#define UTIL_H

#include <memory>

#include "opengl.h"

void loadShaders(GLuint program, const std::string &vsPath, const std::string &fsPath);

#endif // UTIL_H
