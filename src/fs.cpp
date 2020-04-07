#include "fs.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace FS
{

void check_exists(const std::string& path)
{
	if (!std::filesystem::exists(path))
	{
		throw std::runtime_error("File '" + path + "' doesn't exist.");
	}
}

template<typename T>
void check_good(T &file, const std::string &path)
{
	if (!file.good())
	{
		std::stringstream ss;
		ss << "Failed to open '" << path << "'";
		ss << "\nerrno: " << strerror(errno);
		throw std::runtime_error(ss.str());
	}
}
template void check_good<>(std::ifstream&,const std::string&);
template void check_good<>(std::ofstream&,const std::string&);

auto in_file(const std::string& path, bool binary)
{
	check_exists(path);
	auto file {	std::ifstream(path, binary ? std::ios::binary : std::ios::openmode()) };
	check_good(file, path);
	return file;
}

auto out_file(const std::string& path, bool binary)
{
	auto file {	std::ofstream(path, binary ? std::ios::binary : std::ios::openmode()) };
	check_good(file, path);
	return file;
}

std::string readText(const std::string &path)
{
	auto file { in_file(path, false) };
	std::ostringstream buf;
	buf << file.rdbuf();
	return buf.str();
}

std::vector<std::uint8_t> read_bytes(const std::string &path)
{
	auto file { in_file(path, true) };
	file.seekg(0, std::ios::end);
	std::vector<std::uint8_t> buf(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(buf.data()), buf.size());
	return buf;
}

void write_bytes(const std::string& path, const uint8_t *bytes, int count)
{
	auto file { out_file(path, true) };
	file.write(reinterpret_cast<const char*>(bytes), count);
}

}
