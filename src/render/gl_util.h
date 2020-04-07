#ifndef GLUTIL_H
#define GLUTIL_H

#include "render/opengl.h"
#include <string>

void loadShaders(GLuint program, const std::string &vsPath, const std::string &fsPath);

namespace GLUtil
{
	extern GLuint QuadProgram;
	void LoadShaders();
}


#endif // GLUTIL_H
