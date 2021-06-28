#pragma once
#include <string>
#include <rapidjson/document.h>

namespace asyncflow
{
	namespace util
	{
		class JsonUtil
		{
		public:
			static bool ParseJson(const std::string& json_str, rapidjson::Document& doc);
		};
	}
}
