#include "spdlog/sinks/base_sink.h"
#include <array>

namespace spdlog
{
	class lua_logger_sink : public sinks::sink
	{
	public:
		explicit lua_logger_sink(lua_State* L)			
		{
			this->L = L;
			lua_pushvalue(L, -1);	// +1
			logger_ref_ = luaL_ref(L, LUA_REGISTRYINDEX);

			func_names_[static_cast<size_t>(level::trace)] = nullptr;
			func_names_[static_cast<size_t>(level::debug)] = "debug";
			func_names_[static_cast<size_t>(level::info)] = "info";
			func_names_[static_cast<size_t>(level::warn)] = "warning";
			func_names_[static_cast<size_t>(level::err)] = "error";
			func_names_[static_cast<size_t>(level::critical)] = "fatal";
			func_names_[static_cast<size_t>(level::off)] = nullptr;			
		}

		virtual ~lua_logger_sink()
		{
			luaL_unref(L, LUA_REGISTRYINDEX, logger_ref_);
		}

	public:
		void log(const details::log_msg &msg) override
		{
			fmt::memory_buffer formatted;
			formatter_->format(msg, formatted);			
			lua_rawgeti(L, LUA_REGISTRYINDEX, logger_ref_);						// +1
			lua_getfield(L, -1, func_names_[static_cast<size_t>(msg.level)]);	// +1
			if(!lua_isfunction(L, -1))
			{
				lua_pop(L, 2);													// -2
				return;
			}
			lua_pushlstring(L, formatted.data(), formatted.size());				// +1
			if ( lua_pcall(L, 1, 0, 0) != 0 )									// -2
			{				
				auto* error_msg = lua_tostring(L, -1);
				printf("%s\n", error_msg);
				lua_pop(L, 1);
			}
			lua_pop(L, 1);														// -1
			return;
		}

		void set_logger(lua_State* L)
		{
			if (this->L != L)
			{
				printf("%s\n", "[asyncflow] set logger failed: lua_state changed");
				return;
			}
			luaL_unref(L, LUA_REGISTRYINDEX, logger_ref_);
			lua_pushvalue(L, -1);
			logger_ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
		}

		void flush() override {}
		void set_pattern(const std::string &pattern) override
		{
			formatter_ = std::unique_ptr<spdlog::formatter>(new pattern_formatter(pattern));
		}
		void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override
		{
			formatter_ = std::move(sink_formatter);
		}

	private:
		int logger_ref_;
		lua_State* L;
		std::array<const char*, 7> func_names_;
	};
}
