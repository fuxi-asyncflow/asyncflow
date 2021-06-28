#include "util/file.h"

#include <fstream>
using namespace asyncflow::util;

std::string File::ReadAllText(const std::string& file_path)
{
	std::ifstream t(file_path);
	if (t.good())
	{
		return std::string((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>()
		);
	}
	return std::string("");
}
