#ifndef GLUTIL_H
#define GLUTIL_H

#include <string>

#include "render/opengl.h"


void loadShaders(GLuint program, const std::string &vsPath, const std::string &fsPath);

namespace GLUtil {
extern GLuint QuadProgram;
void LoadShaders();
}    // namespace GLUtil

#endif    // GLUTIL_H
