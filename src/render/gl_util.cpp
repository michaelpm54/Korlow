#include "gl_util.h"

#include <stdexcept>
#include <vector>

#include "fs.h"

namespace GLUtil
{

GLuint QuadProgram = GL_NONE;

void LoadShaders()
{
	QuadProgram = glCreateProgram();
	loadShaders(QuadProgram, "assets/shaders/ssquad.vs", "assets/shaders/ssquad.fs");
}

}

void compileShader(GLuint shader)
{
	glCompileShader(shader);
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLint logSize = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		if (logSize == 0 || maxLength == 0)
		{
			throw std::runtime_error("Failed to get shader info log.\nMake sure your OpenGL functions are working correctly.");
		}
		std::vector<GLchar> errorLog(logSize);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
		glDeleteShader(shader);
		throw std::runtime_error(std::string(errorLog.begin(), errorLog.end()));
	}
}

void linkProgram(GLuint program)
{
	glLinkProgram(program);
	GLint success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLint logSize = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		if (logSize == 0 || maxLength == 0)
		{
			throw std::runtime_error("Failed to get program info log.\nMake sure your OpenGL functions are working correctly.");
		}
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);
		glDeleteProgram(program);
		throw std::runtime_error(std::string(errorLog.begin(), errorLog.end()));
	}
}

void loadShaders(GLuint program, const std::string &vsPath, const std::string &fsPath)
{
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	std::string shaderSrcs[2] =
	{
		FS::readText(vsPath),
		FS::readText(fsPath),
	};
	const char *vsSrc = shaderSrcs[0].c_str();
	const char *fsSrc = shaderSrcs[1].c_str();
	glShaderSource(vs, 1, &vsSrc, nullptr);
	glShaderSource(fs, 1, &fsSrc, nullptr);
	compileShader(vs);
	compileShader(fs);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	linkProgram(program);
	glDetachShader(program, vs);
	glDetachShader(program, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
}
