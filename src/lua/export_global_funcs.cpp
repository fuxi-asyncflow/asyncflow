#include "export_global_funcs.h"
#include "lua_manager.h"
#include "lua_common.h"
#include "lua_sink.h"

#ifdef FLOWCHART_DEBUG
#include "debug/websocket_manager.h"
using namespace asyncflow::debug;
#endif

using namespace asyncflow::lua;
using namespace asyncflow::util;

#define LUA_RETURN_NIL(L) \
	lua_pushnil((L)); \
	return 1;

static const char* manager_null_msg = "manager is null, any function shoud called after asyncflow.setup().";
static const char* agr_err_msg = "parse argument failed!";


#pragma region asyncflow_customer_func

bool _get_bool_from_config_dict(lua_State* L, const char* key, bool& b)
{
	lua_getfield(L, 1, key);
	if(lua_isnoneornil(L, -1))
	{
		lua_pop(L, 1);
		return false;
	}

	if (lua_isnumber(L, -1))
	{
		b = ((int)lua_tonumber(L, -1)) ? true : false;
	}
	else
		b = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return true;
}

int asyncflow::lua::setup(lua_State* L)
{
	auto mgr = new LuaManager;
	mgr->L = L;
	mgr->Init();
	if (lua_istable(L, 1))
	{
		bool defer_event = false;
		if(_get_bool_from_config_dict(L, "defer_event", defer_event))
			mgr->SetDeferMode(defer_event);

		lua_getfield(L, 1, "default_timestep");
		if (lua_isnumber(L, -1))
		{
			mgr->SetDefaulTimeInterval((int)lua_tonumber(L, -1));
		}
		lua_pop(L, 1);

		bool node_stop_when_error = false;
		if (_get_bool_from_config_dict(L, "node_stop_when_error", node_stop_when_error))
			mgr->SetNodeStopWhenError(node_stop_when_error);

#ifdef FLOWCHART_DEBUG
		std::string ip = WebsocketManager::IP;
		int port = WebsocketManager::START_PORT;
		lua_getfield(L, 1, "debug_port");
		if (lua_isnumber(L, -1))
		{
			port = (int)lua_tonumber(L, -1);
			ASYNCFLOW_LOG("debug_port is set to {0}.", port);
		}
		lua_getfield(L, 1, "debug_ip");
		if (lua_isstring(L, -1))
		{
			ip = lua_tostring(L, -1);
			ASYNCFLOW_LOG("debug_ip is set to {0}.", ip);
		}
		lua_getfield(L, 1, "debug_name_function");
		if (lua_isstring(L, -1))
		{
			auto method_name = lua_tostring(L, -1);
			Agent::DEBUG_NAME_METHOD = method_name;
			ASYNCFLOW_LOG("debug_name_function is set to {0}.", Agent::DEBUG_NAME_METHOD);
		}
		mgr->GetWebsocketManager().Init(ip, port);
#endif
	}
	else
	{
#ifdef FLOWCHART_DEBUG
		mgr->GetWebsocketManager().Init(WebsocketManager::IP, WebsocketManager::START_PORT);
#endif
	}
	LuaManager::currentManager = mgr;
	LuaExportClass::CreateUserData(L, "manager", LuaManager::currentManager);
	return 1;
}

int asyncflow::lua::config_log(lua_State* L)
{
	if (!lua_isstring(L, 1) || !lua_isstring(L, 2))
	{
		LUA_ERR(L, agr_err_msg);
	}
	auto* path = lua_tostring(L, 1);
	auto* name = lua_tostring(L, 2);
	asyncflow::util::Log::SetLog(path, name);
	return 0;
}

int asyncflow::lua::set_logger(lua_State* L)
{
	if (auto log = spdlog::get("lua_log"))
	{
		for (auto& sink : log->sinks())
		{
			auto lua_sink = std::dynamic_pointer_cast<spdlog::lua_logger_sink>(sink);
			if (lua_sink)
			{
				lua_sink->set_logger(L);
			}
		}
	}
	else
	{
		Log::rotatelogger = spdlog::create<spdlog::lua_logger_sink>("lua_log", L);
	}
	Log::rotatelogger->set_level(Log::LEVEL);
    util::Log::rotatelogger->set_pattern("[asyncflow] %v");	
	return 0;
}

int asyncflow::lua::set_error_handler(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}
	if (!lua_isfunction(L, 1))
	{
		LUA_ERR(L, agr_err_msg);
	}
	lua_settop(L, 1);
	auto ref = LuaManager::currentManager->ErrorHandler;
	if (ref != LUA_NOREF)
	{
		luaL_unref(L, LUA_REGISTRYINDEX, ref);
	}
	LuaManager::currentManager->ErrorHandler = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_pushboolean(L, true);
	return 1;
}

int asyncflow::lua::register_obj(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}

	auto* obj = lua_topointer(L, 1);
	if (obj == nullptr)
	{
		LUA_ERR(L, "1st arg for asyncflow.register should be a userdata or table");
	}
	int tick = Manager::DEFAULT_AGENT_TICK;

	if (lua_istable(L, 2))
	{
		lua_getfield(L, -1, "tick");
		tick = (int)lua_tonumber(L, -1);
	}

	//put obj on the 1 idx of the stack to create agent
	auto* agent = LuaManager::currentManager->RegisterGameObject(const_cast<void*>(obj), tick);
	if (agent == nullptr)
	{
		ASYNCFLOW_ERR("create agent failed in asyncflow.register");
		LUA_RETURN_NIL(L);
	}
	LuaExportClass::CreateUserData(L, "agent", agent);	// +1	
	return 1;
}

int asyncflow::lua::import_charts(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}

	if (!lua_isstring(L, 1))
	{
		LUA_ERR(L, agr_err_msg);
	}
	auto* json_str = lua_tostring(L, 1);
	int counts = LuaManager::currentManager->ImportFile(json_str);
	lua_pushinteger(L, counts);
	return 1;
}

int asyncflow::lua::patch(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}

	if (!lua_isstring(L, 1))
	{
		LUA_ERR(L, agr_err_msg);
	}

	bool in_place = false;
	if(lua_gettop(L) == 2)
	{
		in_place = lua_toboolean(L, 2);
	}

	auto* json_str = lua_tostring(L, 1);
	int counts = LuaManager::currentManager->PatchFromYaml(json_str, in_place);
	lua_pushinteger(L, counts);
	return 1;
}

int asyncflow::lua::import_event(lua_State* L)
{
	auto manager = LuaManager::currentManager;
	if (manager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}
	if (!lua_isstring(L, 1))
	{
		LUA_ERR(L, agr_err_msg);
	}

	auto* path = lua_tostring(L, 1);
	int result = manager->ImportEvent(path);
	if (result)
	{
		std::vector<EventInfo>& event_list = manager->GetEventManager().GetEventList();
		lua_getglobal(L, "asyncflow");
		assert(!lua_isnil(L, -1));
		lua_getfield(L, -1, "EventId");
		assert(!lua_isnil(L, -1));
		for (auto& event_info : event_list)
		{
			lua_pushnumber(L, event_info.id);
			lua_setfield(L, -2, event_info.name.c_str());
		}
	}
	lua_pushinteger(L, result);
	return 1;
}

int asyncflow::lua::event(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}

	int args_count = lua_gettop(L);
	if (args_count < 2)
	{
		LUA_ERR(L, agr_err_msg);
	}  

	auto* obj = lua_topointer(L, 1);
	if (obj == nullptr)
	{
		ASYNCFLOW_ERR("event obj must be table or userdata!");
		lua_pushboolean(L, 0);
		return 1;
	}
	int event_id = (int)lua_tonumber(L, 2);
    if(event_id == 0)
    {
        ASYNCFLOW_ERR("2nd arg for event function should be an non-zero integer");
        lua_pushboolean(L, 0);
        return 1;
    }

	auto event_args = new int[args_count - 2];
	for (int i = args_count - 3; i >= 0; i--)
	{
		event_args[i] = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	bool result = LuaManager::currentManager->Event(event_id, (void*)obj, event_args, args_count - 2);
	lua_pushboolean(L, result);
	return 1;
}

int asyncflow::lua::step(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}

	if (!lua_isnumber(L, 1))
	{
		LUA_ERR(L, agr_err_msg);
	}
	int step_time = (int)lua_tonumber(L, 1);
	LuaManager::currentManager->Step(step_time);
	return 0;
}

int asyncflow::lua::deregister_obj(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}

	bool result = false;
	auto* obj = lua_topointer(L, 1);
	if (obj == nullptr)
	{
		LUA_ERR(L, "1st arg for asyncflow.deregister should be a userdata or table");
	}
	result = LuaManager::currentManager->UnregisterGameObject(const_cast<void*>(obj));
	lua_pushboolean(L, result);
	return 1;
}

int asyncflow::lua::exit(lua_State* L)
{
	if (LuaManager::currentManager != nullptr)
	{
		delete LuaManager::currentManager;
		LuaManager::currentManager = nullptr;
	}
	return 0;
}

int asyncflow::lua::get_current_manager(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}

	LuaExportClass::CreateUserData(L, "manager", LuaManager::currentManager);
	return 1;
}

int asyncflow::lua::attach(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}
	auto* obj = lua_topointer(L, 1);
	if (obj == nullptr)
	{
		LUA_ERR(L, "1st arg for asyncflow.attach should be a userdata or table");
	}
	if (!lua_isstring(L, 2))
	{
		LUA_ERR(L, "2rd arg for asyncflow.attach should be a chart_name");
	}

	auto* chart_name = lua_tostring(L, 2);
	auto chart = (LuaChart*)LuaManager::currentManager->AttachChart(const_cast<void*>(obj), chart_name);
	if (chart != nullptr && lua_istable(L, 3))
	{
		lua_settop(L, 3);
		chart->SetInitTable(L);
	}
	if (chart != nullptr)
	{
		LuaExportClass::CreateUserData(L, "chart", chart);
		ASYNCFLOW_LOG("attach chart {0} {1}", obj, chart_name);
	}
	else
		lua_pushnil(L);
	return 1;
}

int asyncflow::lua::remove(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}
	auto* obj = lua_topointer(L, 1);
	if (obj == nullptr)
	{
		LUA_ERR(L, "1st arg for asyncflow.remove should be a userdata or table");
	}
	if (!lua_isstring(L, 2))
	{
		LUA_ERR(L, agr_err_msg);
	}
	auto* chart_name = lua_tostring(L, 2);
	auto agent = LuaManager::currentManager->GetAgent(const_cast<void*>(obj));
	if (agent == nullptr)
	{
		ASYNCFLOW_WARN("object is not registered to asyncflow");
		lua_pushboolean(L, false);
		return 1;
	}
	bool result = agent->RemoveChart(chart_name);
	lua_pushboolean(L, result);
	return 1;
}

int asyncflow::lua::start(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}
	auto* obj = lua_topointer(L, 1);
	if (obj == nullptr)
	{
		LUA_ERR(L, "1st arg for asyncflow.start should be a userdata or table");
	}
	if (lua_gettop(L) > 1 && !lua_istable(L, 2))
	{
		LUA_ERR(L, agr_err_msg);
	}
	auto agent = LuaManager::currentManager->GetAgent(const_cast<void*>(obj));
	if (agent == nullptr)
	{
		ASYNCFLOW_WARN("object is not registered to asyncflow");
		lua_pushboolean(L, 0);
		return 1;
	}
	if (lua_gettop(L) == 1)
	{
		agent->Start();
		lua_pushboolean(L, 0);
		return 1;
	}

	lua_pushnil(L);
	while (lua_next(L, 2) != 0) {
		if (lua_isstring(L, -1))
		{
			auto chart_name = lua_tostring(L, -1);
			agent->StartChart(chart_name);
		}
		lua_pop(L, 1);
	}
	lua_pushboolean(L, 1);
	return 1;
}

int asyncflow::lua::stop(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		LUA_ERR(L, manager_null_msg);
	}
	auto* obj = lua_topointer(L, 1);
	if (obj == nullptr)
	{
		LUA_ERR(L, "1st arg for asyncflow.stop should be a userdata or table");
	}
	if (lua_gettop(L) > 1 && !lua_istable(L, 2))
	{
		LUA_ERR(L, agr_err_msg);
	}
	auto agent = LuaManager::currentManager->GetAgent(const_cast<void*>(obj));
	if (agent == nullptr)
	{
		lua_pushboolean(L, 0);
		return 1;
	}
	if (lua_gettop(L) == 1)
	{
		agent->Stop();
		lua_pushboolean(L, 0);
		return 1;
	}

	lua_pushnil(L);
	while (lua_next(L, 2) != 0) {
		if (lua_isstring(L, -1))
		{
			auto chart_name = lua_tostring(L, -1);
			agent->StopChart(chart_name);
		}
		lua_pop(L, 1);
	}
	lua_pushboolean(L, 1);
	return 1;
}

int asyncflow::lua::set_node_func(lua_State* L)
{
	lua_getglobal(L, "asyncflow");					// +1
	lua_getfield(L, -1, "node_funcs");	// +1
	lua_pushvalue(L, 1);		// +1
	lua_pushvalue(L, 2);		// +1
	lua_rawset(L, -3);		// -2

	return 0;
}
#pragma endregion asyncflow_customer_func

#pragma region asyncflow_inner_func
int asyncflow::lua::wait(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
		return 0;

	int tm = (int)(1000 * lua_tonumber(L, 1));
	LuaManager::currentManager->Wait(tm);
	return 0;
}

int asyncflow::lua::get_var(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
		return 0;

	int var_id;

	if (lua_type(L, 1) == LUA_TSTRING)
	{
		size_t len;
		auto name = lua_tolstring(L, 1, &len);
		var_id = LuaManager::currentManager->GetCurrentNode()->GetChart()->GetData()->GetVarIndex(std::string(name, len));
	}
	else
	{
		var_id = (int)lua_tonumber(L, 1);
	}

	LuaManager::currentManager->GetVar(L, var_id);
	return 1;
}

int asyncflow::lua::set_var(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
		return 0;
	//TODO  optimization
	int var_id;

	if(lua_type(L, 1) == LUA_TSTRING)
	{
		size_t len;
		auto name = lua_tolstring(L, 1, &len);		
		var_id = LuaManager::currentManager->GetCurrentNode()->GetChart()->GetData()->GetVarIndex(std::string(name, len));
	}
	else
	{
		var_id = (int)lua_tonumber(L, 1);	    
	}
	
	lua_remove(L, 1);   // set var in 1 idx of stack
	LuaManager::currentManager->SetVar(L, var_id);
	if (lua_type(L, 1) == LUA_TNUMBER)
	{
		lua_pushboolean(L, lua_tonumber(L, 1) != 0);
	}
	else
	{
		lua_pushboolean(L, lua_toboolean(L, 1));
	}
	return 1;
}

int asyncflow::lua::call_sub(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}
	auto chart_name = lua_tostring(L, 1);
	auto* obj = lua_topointer(L, 2);
	if (obj == nullptr)
	{
		ASYNCFLOW_ERR("call_sub obj must be table or userdata");
		lua_pushboolean(L, 0);
		return 1;
	}
	int arg_count = lua_gettop(L) - 2;
	lua_remove(L, 1);   //obj set on the 1 idx of the stack
	bool result = LuaManager::currentManager->Subchart(chart_name, (void*)obj, L, arg_count);
	return 1;
}

int asyncflow::lua::ret(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}
	LuaManager::currentManager->Return(L);
	return 1;
}

int asyncflow::lua::wait_event(lua_State* L)
{
	auto luaManager = LuaManager::currentManager;
	if (luaManager == nullptr)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	bool res = false;
	auto* obj = lua_topointer(L, 1);
	int event_id = (int)lua_tonumber(L, 2);
	if (event_id < 1)
	{
		auto* str = lua_tostring(L, 2);
		ASYNCFLOW_ERR("event_id can not be {0}", str == nullptr ? "null" : str);
		lua_pushboolean(L, 0);
		return 1;
	}
	if (obj == nullptr)
	{
		ASYNCFLOW_ERR("wait event obj must be table or userdata");
		lua_pushboolean(L, 0);
		return 1;
	}
	Agent* agent = luaManager->TryGetAgent((void*)obj);
	res = luaManager->WaitEvent(agent, event_id);
	lua_pushboolean(L, res);
	return 1;
}

int asyncflow::lua::get_event_param(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}

	int event_id = (int)lua_tonumber(L, 1);
	int param_id = (int)lua_tonumber(L, 2);
	LuaManager::currentManager->GetEventParam(L, event_id, param_id);
	return 1;
}

int asyncflow::lua::wait_all(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
		return 0;

	int argc = (int)lua_gettop(L);
	std::vector<int> ids(argc);
	
	for (int i = 0; i < argc; i++)
	{
		ids[i] = (int)lua_tonumber(L, i + 1);
	}
	LuaManager::currentManager->WaitAll(ids);
	
	return 0;
}

int asyncflow::lua::callback_func(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	long long node_address = lua_tointeger(L, lua_upvalueindex(1));
	bool result = LuaManager::currentManager->ContinueAsyncNode(node_address, L);
	lua_pushboolean(L, result);
	return 1;
}

int asyncflow::lua::callback(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}

	long long node_address = LuaManager::currentManager->CreateAsyncContext();
	lua_pushinteger(L, node_address);
	lua_pushcclosure(L, callback_func, 1);
	return 1;
}

int asyncflow::lua::time(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}
	lua_pushinteger(L, LuaManager::currentManager->Now());
	return 1;
}

int asyncflow::lua::stop_node(lua_State* L)
{
	int top = lua_gettop(L);
	if (LuaManager::currentManager == nullptr || top == 0)
	{
		lua_pushboolean(L, 0);
		return 1;
	}
	std::vector<int> ids(top);	
	LuaManager::currentManager->StopNode(ids);	
	lua_pushboolean(L, 1);
	return 1;
}

int asyncflow::lua::stop_flow(lua_State* L)
{
	int top = lua_gettop(L);
	if (LuaManager::currentManager == nullptr || top == 0)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	std::vector<int> ids(top);
	for (int i = 0; i < top; i++)
	{
		ids[i] = (int)lua_tointeger(L, i + 1);
	}
	LuaManager::currentManager->StopFlow(ids);	
	lua_pushboolean(L, 1);
	return 1;
}
#pragma endregion asyncflow_inner_func
