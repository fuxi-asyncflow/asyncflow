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
            static bool Exists(const std::string& file_path);
		};

		class Base64
		{
			// from https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp/
		public:
			static std::string base64_encode(std::string const& s, bool url = false);
			static std::string base64_encode_pem(std::string const& s);
			static std::string base64_encode_mime(std::string const& s);

			static std::string base64_decode(std::string const& s, bool remove_linebreaks = false);
			static std::string base64_encode(unsigned char const*, size_t len, bool url = false);
		};

		class UuidUtil
		{
		public:
			static bool NtoD(const char* n, char* d);
			static bool DtoN(const char* d, char* n);
		};
	}
}