#pragma once
#include <string>
#include <rapidjson/document.h>


namespace c4
{
    namespace yml
    {
        struct Location;
        struct Callbacks;

    }
}

namespace asyncflow
{

	namespace util
	{        
		class JsonUtil
		{
		public:
			static bool ParseJson(const std::string& json_str, rapidjson::Document& doc);
		};

        struct YamlErrorHandler
        {
            // this will be called on error
            void on_error(const char* msg, size_t len, c4::yml::Location loc);
           

            // bridge
            c4::yml::Callbacks callbacks();
           
            static void s_error(const char* msg, size_t len, c4::yml::Location loc, void* this_);
            

            // save the default callbacks for checking
            YamlErrorHandler() = default;
            ~YamlErrorHandler() = default;
        };
	}
}
