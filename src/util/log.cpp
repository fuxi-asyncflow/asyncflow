#include "util/log.h"
//#include "util/python_sink.h"
#include "spdlog_setup/conf.h"

std::shared_ptr<spdlog::logger> asyncflow::util::Log::rotatelogger = nullptr;
bool asyncflow::util::Log::init_ = false;

#ifdef NDEBUG
spdlog::level::level_enum asyncflow::util::Log::LEVEL = spdlog::level::info;
#else
spdlog::level::level_enum asyncflow::util::Log::LEVEL = spdlog::level::debug;
#endif


namespace asyncflow
{
	namespace util
	{
		void Log::Init()
		{
			//rotatelogger = spdlog::rotating_logger_mt("fctlog", "flowchart_log.txt", 5 * 1024 * 1024, 3);
#if ENABLE_LOG
			if (!init_)
			{
				init_ = true;
				SetLog("", "");
			}
#endif
		}

		void Log::SetLog(const std::string& path, const std::string& log_name)
		{
			spdlog::drop_all();			
			if (!path.empty())
			{
				try 
				{					
					// if the file not find, path regards as config string
					spdlog_setup::from_file(path);
					rotatelogger = spdlog::get(log_name);
					if (rotatelogger != nullptr)
					{
						printf("[asyncflow] config spdlog success, use %s in %s\n", log_name.c_str(), path.c_str());
						return;
					}
				}
				catch (const std::exception& e) 
				{
					printf("[asyncflow] Read log config from %s error, the reason is %s!\n", path.c_str(), e.what() );
				}
			}

			// default config
			rotatelogger = spdlog::get("console");
			if (rotatelogger == nullptr)
			{
				rotatelogger = spdlog::stdout_color_st("console");
				rotatelogger->set_level(LEVEL);
				rotatelogger->set_pattern("[%H:%M:%S %e] [thread %t] %^ %v %$");
			}
		}
	}
}
