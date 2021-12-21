#pragma once
#include "export_lua.h"

namespace asyncflow
{
	namespace lua
	{
		int setup(lua_State* L);
		int config_log(lua_State* L);
		int register_obj(lua_State* L);
		int import_charts(lua_State* L);
		int import_event(lua_State* L);
		int set_error_handler(lua_State* L);
		int event(lua_State* L);
		int step(lua_State* L);
		int deregister_obj(lua_State* L);
		int exit(lua_State* L);
		int get_current_manager(lua_State* L);
		int attach(lua_State* L);
		int remove(lua_State* L);
		int start(lua_State* L);
		int stop(lua_State* L);

		int wait(lua_State* L);
		int get_var(lua_State* L);
		int set_var(lua_State* L);
		int call_sub(lua_State* L);
		int ret(lua_State* L);
		int wait_event(lua_State* L);
		int get_event_param(lua_State* L);
		int wait_all(lua_State* L);
		int callback_func(lua_State* L);
		int callback(lua_State* L);
		int time(lua_State* L);
		int stop_node(lua_State* L);
		int stop_flow(lua_State* L);

	}
}

