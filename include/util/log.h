#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#ifndef ENABLE_LOG
#define ENABLE_LOG 1
#endif

namespace asyncflow 
{
	namespace util
	{
		class Log
		{
			
			static bool init_;	
		public:
			static std::shared_ptr<spdlog::logger> rotatelogger;
			static spdlog::level::level_enum LEVEL;
			
		public:			
			static void Init();
			static void Destroy();
			static void SetLog(const std::string& name, const std::string& log_name);

			template<typename... Args>
			inline static void DBG(const char *fmt, const Args &... args)
			{				
				rotatelogger->debug(fmt, args...);
			}

			template<typename... Args>
			inline static void LOG(const char *fmt, const Args &... args)
			{				
				rotatelogger->info(fmt, args...);
			}

			template<typename... Args>
			inline static void WARN(const char *fmt, const Args &... args)
			{				
				rotatelogger->warn(fmt, args...);
			}

			template<typename... Args>
			inline static void ERR(const char *fmt, const Args &... args)
			{				
				rotatelogger->error(fmt, args...);
			}
		};
	}
}

#if ENABLE_LOG
#if NDEBUG
	#define ASYNCFLOW_DBG(...)	((void)0)
#else
	#define ASYNCFLOW_DBG(...) asyncflow::util::Log::DBG(__VA_ARGS__)
#endif
#define ASYNCFLOW_LOG(...) asyncflow::util::Log::LOG(__VA_ARGS__)
#define ASYNCFLOW_WARN(...) asyncflow::util::Log::WARN(__VA_ARGS__)
#define ASYNCFLOW_ERR(...) asyncflow::util::Log::ERR(__VA_ARGS__)

#else
#define ASYNCFLOW_LOG(...)	((void)0)
#define ASYNCFLOW_DBG(...)	((void)0)
#define ASYNCFLOW_WARN(...) ((void)0)
#define ASYNCFLOW_ERR(...)	((void)0)
#endif
