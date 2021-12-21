#pragma once
#include "export_lua.h"

namespace asyncflow
{
	namespace lua
	{
		int mgr_register_obj(lua_State* L);
		int mgr_import_charts(lua_State* L);
		int mgr_import_event(lua_State* L);
		int mgr_set_error_handler(lua_State* L);
		int mgr_step(lua_State* L);
		int mgr_event(lua_State* L);
		int mgr_exit(lua_State* L);
		int mgr_deregister_obj(lua_State* L);

		int agent_attach_chart(lua_State* L);
		int agent_remove_chart(lua_State* L);
		int agent_get_charts(lua_State* L);
		int agent_start(lua_State* L);
		int agent_stop(lua_State* L);
		int agent_get_obj(lua_State* L);

		int chart_set_callback(lua_State* L);
	}
}
