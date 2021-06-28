#pragma once

#ifdef _WINDLL
#define ASYNCFLOW_API __declspec(dllexport)
#else
#define ASYNCFLOW_API
#endif

#include"util/log.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

typedef int Ref;

#ifdef NDEBUG
#define CheckLuaStack(delta)	((void)0);
#else
#define STRINGFY(X, Y) X##Y
#define CheckLuaStack(delta) asyncflow::lua::LuaStackCheck STRINGFY(check, __COUNTER__)(__FILE__,__LINE__, L, delta)
#endif

#define LUA_ARG_ERR(L, arg, msg)					\
do {												\
    ASYNCFLOW_ERR(msg);								\
    return luaL_argerror((L), (arg), (msg));		\
} while (false)

#define LUA_ERR(L, msg)								\
do {												\
	ASYNCFLOW_ERR(msg);								\
	return luaL_error((L), (msg));					\
} while (false)

namespace asyncflow
{
	namespace lua
	{
		class LuaStackCheck
		{
		public:
			LuaStackCheck(const char* file_name, int line_num, lua_State* L, int delta)
				:file_name(file_name), line_num(line_num), l(L), delta_top(delta)
			{
				original_top = lua_gettop(L);
			};

			~LuaStackCheck() noexcept(false)
			{
				int current_top = lua_gettop(l);
				bool result = current_top == original_top + delta_top;
				if (!result)
				{
					ASYNCFLOW_WARN("lua stack is not balance. stack watching started in {0}:{1}", file_name, line_num);
				}
				assert(result);
			}

		private:
			int original_top;
			int delta_top;
			lua_State* l;
			const std::string file_name;
			int line_num;
		};

		std::string ToString(lua_State* L, int idx);  //equal to luaL_tolstring in lua 5.3
	}
}
