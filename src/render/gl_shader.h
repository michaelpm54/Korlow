#ifndef KORLOW_RENDER_GL_SHADER_H
#define KORLOW_RENDER_GL_SHADER_H

#include <string>

#include "render/gl.h"

GLuint load_shader(const std::string &vertex_path, const std::string &fragment_path);

#endif    // KORLOW_RENDER_GL_SHADER_H
