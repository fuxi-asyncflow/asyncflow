#include "export_global_funcs.h"
#include "lua_manager.h"
#include "lua_common.h"

#ifdef FLOWCHART_DEBUG
#include "debug/websocket_manager.h"
using namespace asyncflow::debug;
#endif

using namespace asyncflow::lua;

#define LUA_RETURN_NIL(L) \
	lua_pushnil((L)); \
	return 1;

static const char* manager_null_msg = "manager is null, any function shoud called after asyncflow.setup().";
static const char* agr_err_msg = "parse argument failed!";


#pragma region asyncflow_customer_func

int asyncflow::lua::setup(lua_State* L)
{
	auto mgr = new LuaManager;
	mgr->L = L;
	mgr->Init();
	if (lua_istable(L, 1))
	{
		lua_getfield(L, 1, "loop_check");
		if (lua_isboolean(L, -1))
		{
			bool flag = lua_toboolean(L, -1);
			mgr->GetExecutor().SetLoopCheck(flag);
			ASYNCFLOW_LOG("loop_check is set to {0}.", flag);
		}
		lua_getfield(L, 1, "immediate_subchart");
		if (lua_isboolean(L, -1))
		{
			bool flag = lua_toboolean(L, -1);
			mgr->SetImmediateSub(flag);
			ASYNCFLOW_LOG("immediate_subchart is set to {0}.", flag);
		}
		lua_getfield(L, 1, "default_timestep");
		if (lua_isnumber(L, -1))
		{
			int step = (int)lua_tonumber(L, -1);
			if (step <= 0)
				ASYNCFLOW_WARN("default_timestep must be larger than 0.");
			else
			{
				mgr->SetDefaulTimeInterval(step);
				ASYNCFLOW_LOG("default_timestep is set to {0}.", step);
			}
		}

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
	int tick = 0;
	bool start_flag = true;

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
	if (args_count < 2 || !lua_isnumber(L, 2))
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

	int var_id = (int)lua_tonumber(L, 1);
	LuaManager::currentManager->GetVar(L, var_id);
	return 1;
}

int asyncflow::lua::set_var(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
		return 0;

	int var_id = (int)lua_tonumber(L, 1);
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
		ASYNCFLOW_ERR("event_id can not be {0}", event_id);
		lua_pushboolean(L, 0);
		return 1;
	}
	if (obj == nullptr)
	{
		ASYNCFLOW_ERR("wait event obj must be table or userdata");
		lua_pushboolean(L, 0);
		return 1;
	}
	Agent* agent = luaManager->GetAgent((void*)obj);
	if (agent == nullptr)
	{
		ASYNCFLOW_ERR("wait event obj {} is not registered", obj);
		agent = luaManager->RegisterGameObject(const_cast<void*>(obj), Manager::DEFAULT_AGENT_TICK);
	}
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
	LuaManager::currentManager->GetEventParam(L, event_id, param_id - 1);
	return 1;
}

int asyncflow::lua::wait_all(lua_State* L)
{
	if (LuaManager::currentManager == nullptr)
		return 0;

	int argc = (int)lua_gettop(L);
	int* ids = new int[argc];
	for (int i = 0; i < argc; i++)
	{
		ids[i] = (int)lua_tonumber(L, i + 1);
	}
	LuaManager::currentManager->WaitAll(span<int>(ids, argc));
	delete[] ids;
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
	bool result = LuaManager::currentManager->AsyncCallback(node_address, L);
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

	int* ids = new int[top];
	for (int i = 0; i < top; i++)
	{
		ids[i] = (int)lua_tointeger(L, i + 1);
	}
	LuaManager::currentManager->StopNode(span<int>(ids, top));
	delete[] ids;
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

	int* ids = new int[top];
	for (int i = 0; i < top; i++)
	{
		ids[i] = (int)lua_tointeger(L, i + 1);
	}
	LuaManager::currentManager->StopFlow(span<int>(ids, top));
	delete[] ids;
	lua_pushboolean(L, 1);
	return 1;
}
#pragma endregion asyncflow_inner_func
