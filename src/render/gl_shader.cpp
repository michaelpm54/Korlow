#include <sstream>
#include <stdexcept>
#include <vector>

#include "fs.h"
#include "render/gl.h"

void check_compile_error(GLuint handle)
{
    int success = GL_FALSE;

    /* Everything is fine */
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (success == GL_TRUE) {
        return;
    }

    /* Get log length */
    int maxLength;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &maxLength);

    /* Get log */
    int length;
    std::vector<GLchar> log(maxLength);
    glGetShaderInfoLog(handle, maxLength, &length, &log[0]);

    /* Delete because we won't be using it */
    glDeleteShader(handle);

    /* Put the error into a string  character by character... */
    std::stringstream error;
    error << "Compile log: \n";
    for (const GLchar &c : log) {
        error << c;
    }
    error << '\n';

    /* Just print the error */
    fprintf(stderr, "Error: %s\n", error.str());
}

void check_link_error(GLuint handle)
{
    int success = GL_TRUE;

    /* Everything is fine */
    glGetProgramiv(handle, GL_LINK_STATUS, &success);
    if (success == GL_TRUE) {
        return;
    }

    /* Get log length */
    int maxLength;
    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &maxLength);

    /* Get log */
    int length;
    std::vector<GLchar> log(maxLength);
    glGetProgramInfoLog(handle, maxLength, &length, &log[0]);

    /* Delete becase we won't be using it */
    glDeleteProgram(handle);

    /* Put the error into a string  character by character... */
    std::stringstream error;
    error << "Link log: \n";
    for (const GLchar &c : log) {
        error << c;
    }
    error << std::endl;

    /* Just print the error */
    fprintf(stderr, "Error: %s\n", error.str());
}

GLuint compile_shader(GLenum type, const std::string &shader_path)
{
    /* Get shader source */
    auto src = FS::readText(shader_path);
    auto srcCStr = src.c_str();

    /* Create shader and compile */
    GLuint handle = glCreateShader(type);
    glShaderSource(handle, 1, &srcCStr, nullptr);
    glCompileShader(handle);

    check_compile_error(handle);

    return handle;
}

GLuint load_shader(const std::string &vertex_path, const std::string &fragment_path)
{
    static constexpr GLenum kShaderTypes[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

    GLuint program = glCreateProgram();

    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_path);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_path);

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);
    check_link_error(program);

    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}
