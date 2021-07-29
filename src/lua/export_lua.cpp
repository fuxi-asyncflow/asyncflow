#include "export_lua.h"
#include "lua_manager.h"
#include <algorithm>
#include "core/agent.h"
#include "export_global_funcs.h"
#include "export_class_funcs.h"

using namespace asyncflow::lua;
using namespace asyncflow::core;

std::unordered_map<std::string, Ref> LuaExportClass::mt_dict;


/////////////////////////////////////////////////////////////////////////////////////////

void LuaExportModule::AddFunction(const std::string& func_name, lua_CFunction func)
{
	module_funcs_[func_name] = func;
}

void LuaExportModule::Export(lua_State* L)
{
	std::vector<luaL_Reg> funcs;
	for (auto kv : module_funcs_)
	{
		luaL_Reg l;
		auto* str = new char[kv.first.length() + 1];
		strcpy_s(str, kv.first.length() + 1, kv.first.c_str());
		l.name = str;
		l.func = kv.second;
		funcs.push_back(l);
	}

	luaL_Reg empty;
	empty.name = nullptr;
	empty.func = nullptr;
	funcs.push_back(empty);

	luaL_register(L, module_name_.c_str(), &funcs[0]);
	for (auto& func : funcs)
	{
		delete[] func.name;
	}

	lua_newtable(L);   //table for EventId
	lua_setfield(L, -2, "EventId");
	lua_newtable(L);   //table node_funcs to store node functions
	lua_setfield(L, -2, "node_funcs");
}

//////////////////////////////////////////////////////////////////////////////////////
int luaopen_asyncflow(lua_State *L)
{
	asyncflow::util::Log::Init();
	LuaExportModule module("asyncflow");

	module.AddFunction("setup", setup);	
	module.AddFunction("config_log", config_log);
	module.AddFunction("import_charts", import_charts);
	module.AddFunction("import_event", import_event);
	module.AddFunction("set_error_handler", set_error_handler);
	module.AddFunction("register", register_obj);
	module.AddFunction("step", step);
	module.AddFunction("event", event);
	module.AddFunction("deregister", deregister_obj);
	module.AddFunction("exit", exit);
	module.AddFunction("get_current_manager", get_current_manager);
	module.AddFunction("attach", attach);
	module.AddFunction("remove", remove);
	module.AddFunction("start", start);
	module.AddFunction("stop", stop);

	module.AddFunction("wait", wait);
	module.AddFunction("set_var", set_var);
	module.AddFunction("get_var", get_var);
	module.AddFunction("call_sub", call_sub);
	module.AddFunction("ret", ret);
	module.AddFunction("wait_event", wait_event);
	module.AddFunction("get_event_param", get_event_param);
	module.AddFunction("wait_all", wait_all);
	module.AddFunction("callback", callback);
	module.AddFunction("time", time);
	module.AddFunction("stop_node", stop_node);
	module.AddFunction("stop_flow", stop_flow);

	auto manager_cls = LuaExportClass(L, "manager");
	manager_cls.AddFunction("register", mgr_register_obj);
	manager_cls.AddFunction("import_charts", mgr_import_charts);
	manager_cls.AddFunction("import_event", mgr_import_event);
	manager_cls.AddFunction("set_error_handler", mgr_set_error_handler);
	manager_cls.AddFunction("step", mgr_step);
	manager_cls.AddFunction("event", mgr_event);
	manager_cls.AddFunction("exit", mgr_exit);
	manager_cls.AddFunction("deregister", mgr_deregister_obj);

	auto agent_cls = LuaExportClass(L, "agent");
	agent_cls.AddFunction("attach", agent_attach_chart);
	agent_cls.AddFunction("remove", agent_remove_chart);
	agent_cls.AddFunction("get_charts", agent_get_charts);
	agent_cls.AddFunction("start", agent_start);
	agent_cls.AddFunction("stop", agent_stop);
	agent_cls.AddFunction("get_obj", agent_get_obj);

	auto chart_cls = LuaExportClass(L, "chart");
	chart_cls.AddFunction("set_callback", chart_set_callback);
	module.Export(L);
	return 0;
}

void* get_manager()
{
	return LuaManager::currentManager;
}