#pragma once
#include <string>
namespace asyncflow
{
	namespace util
	{
		class File
		{
		public:
			static std::string ReadAllText(const std::string& file_path);
		};
	}
}