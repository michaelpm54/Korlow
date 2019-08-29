#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <GL/glew.h>

std::string readFileText(const std::string &filePath, int &size)
{
	FILE *file = fopen(filePath.c_str(), "rb");
	if (!file)
	{
		throw std::runtime_error("Failed to open file '" + filePath + "':\n" + std::string(strerror(errno)));
	}
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	std::string buffer;
	buffer.resize(size);
	fseek(file, 0, SEEK_SET);
	fread(buffer.data(), 1, size, file);
	fclose(file);

	return buffer;
}

std::unique_ptr<uint8_t[]> readFileBytes(const std::string &filePath, int &size)
{
	FILE *file = fopen(filePath.c_str(), "rb");
	if (!file)
	{
		throw std::runtime_error("Failed to open file '" + filePath + "':\n" + std::string(strerror(errno)));
	}
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	std::unique_ptr<uint8_t[]> bytes(new uint8_t[size]);
	fseek(file, 0, SEEK_SET);
	fread(bytes.get(), 1, size, file);
	fclose(file);

	return bytes;
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
		std::vector<GLchar> errorLog(maxLength);
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
	int size;
	std::string shaderSrcs[2] =
	{
		readFileText(vsPath, size),
		readFileText(fsPath, size),
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
