#include "fs.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>

namespace FS
{

class File
{
public:
	File(const std::string &path, bool binary)
	{
		if (binary)
			fptr = fopen(path.c_str(), "rb");
		else
			fptr = fopen(path.c_str(), "r");

		if (!fptr)
		{
			int err = errno;

			std::stringstream ss;
			ss << "Failed to open " << std::filesystem::current_path().string() + path << ", " << err << ": " << std::string(strerror(err));

			throw std::runtime_error(ss.str());
			fclose(fptr);
		}
	}

	std::string readText()
	{
		fseek(fptr, 0, SEEK_END);
		std::string text(ftell(fptr), ' ');
		fseek(fptr, 0, SEEK_SET);
		fread(text.data(), 1, text.size(), fptr);
		return text;
	}

	std::vector<std::uint8_t> readBytes()
	{
		fseek(fptr, 0, SEEK_END);
		std::vector<std::uint8_t> bytes(ftell(fptr));
		fseek(fptr, 0, SEEK_SET);
		fread(bytes.data(), 1, bytes.size(), fptr);
		return bytes;
	}

	~File()
	{
		fclose(fptr);
	}

private:
	FILE *fptr { nullptr };
};

std::string readText(const std::string &filePath)
{
	return File(filePath, false).readText();
}

std::vector<std::uint8_t> readBytes(const std::string &filePath)
{
	return File(filePath, true).readBytes();
}

}
