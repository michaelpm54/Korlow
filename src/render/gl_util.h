#ifndef GLUTIL_H
#define GLUTIL_H

#include <string>

#include "opengl.h"

void loadShaders(GLuint program, const std::string &vsPath, const std::string &fsPath);

#endif // GLUTIL_H
