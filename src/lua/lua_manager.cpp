#include "lua_manager.h"
#include "lua_chartdata.h"
#include "lua_chart.h"

using namespace asyncflow::core;
using namespace asyncflow::lua;

/////////////////////////////////////////////////////////////////////////////////////
#ifdef BUILD_MULTITHREAD
thread_local LuaManager* LuaManager::currentManager = nullptr;
#else
LuaManager* LuaManager::currentManager = nullptr;
#endif

void LuaManager::Init()
{
	if (is_init || (L == nullptr))
		return;

	LuaManager::currentManager = this;
	lua_pushcfunction(L, LuaManager::ErrorFunction);	//  +1
	ErrorHandler = luaL_ref(L, LUA_REGISTRYINDEX);		//  -1
	lua_newtable(L);									//  +1
	FunctionRef = luaL_ref(L, LUA_REGISTRYINDEX);		//  -1
	lua_newtable(L);
	ObjectRef = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_newtable(L);									//  +1
	ChartVariableRef = luaL_ref(L, LUA_REGISTRYINDEX);  //  -1
	is_init = true;
	return;
}

Agent* LuaManager::RegisterGameObject(void* obj, int tick)
{
	if (tick <= 0)
		tick = DEFAULT_AGENT_TICK;

	if (agent_manager_.GetAgent(obj) != nullptr)
	{
		ASYNCFLOW_LOG("object has registered to asyncflow");
		return nullptr;
	}
	auto* agent = agent_manager_.Register(obj);
	agent->SetTickInterval(tick);
	return agent;
}

bool LuaManager::UnregisterGameObject(void* obj)
{
	auto* agent = agent_manager_.GetAgent(obj);
	return Manager::UnregisterGameObject(agent);
}

bool LuaManager::Event(int event_id, void* obj, int* args, int arg_count, bool immediate)
{	
	return Manager::Event(event_id, TryGetAgent(obj), (void*)args, arg_count, immediate);
}

std::pair<bool, std::vector<std::string>> LuaManager::RunScript(const char * str)
{
	CheckLuaStack(0);
	lua_rawgeti(L, LUA_REGISTRYINDEX, ErrorHandler);									//  +1
	int error_handler = lua_gettop(L);
	bool load_flag = luaL_loadstring(L, str);											//  +1
	std::vector<std::string> result;
	if (load_flag != 0)
	{
		auto* err_msg = lua_tostring(L, -1);
		ASYNCFLOW_ERR("gm load string error: {0}", err_msg);
		result.push_back(err_msg);
		lua_pop(L, 2);																	//  -2
		return std::pair<bool, std::vector<std::string>>(false, result);
	}

	if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0)   //no error_handle
	{
		auto* err_msg = lua_tostring(L, -1);
		ASYNCFLOW_ERR("gm call error: {0}", err_msg);
		result.push_back(err_msg);
		lua_pop(L, 2);																	//  -2
		return std::pair<bool, std::vector<std::string>>(false, result);
	}
	int current_top = lua_gettop(L);
	for (auto idx = error_handler + 1; idx <= current_top; idx++)
	{
		result.push_back(ToString(L, idx));
	}
	lua_settop(L, error_handler - 1);
	return std::pair<bool, std::vector<std::string>>(true, result);
}

void LuaManager::SetVar(lua_State* L, int var_id)
{
	auto chart = static_cast<LuaChart*>(GetCurrentNode()->GetChart());
	chart->SetVar(L, var_id);
	return;
}

void LuaManager::GetVar(lua_State* L, int var_id)
{
	auto chart = static_cast<LuaChart*>(GetCurrentNode()->GetChart());
	chart->GetVar(L, var_id);
}

void LuaManager::GetEventParam(lua_State* L, int event_id, int param_idx)
{
	assert(current_event_ != nullptr);
	if (event_id != current_event_->Id())
	{
		ASYNCFLOW_WARN("event id unmatch when get event param");
	}
	auto ref = ((AsyncEvent*)current_event_)->GetEventParam(param_idx);

	lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
}

void LuaManager::Return(lua_State* L)
{
	auto node = GetCurrentNode();
	auto* chart = static_cast<LuaChart*>(node->GetChart());
	chart->Return(L);
}

bool LuaManager::AsyncCallback(long long context, lua_State* L)
{
	CheckLuaStack(0);
	auto node = (core::Node*)context;
	if (async_manager_.IsNodeWaiting(node))
	{
		ASYNCFLOW_DBG("async callback for node {0} {1}[{2}]", (void*)node, node->GetChart()->Name(), node->GetId());
		async_manager_.RemoveNode(node);
		async_manager_.ActivateNode(node);
		auto const var_id = node->GetData()->GetVarId();
		if (var_id >= 0)
			((LuaChart*)node->GetChart())->SetVar(L, var_id);
		bool ret = lua_toboolean(L, -1);
		node->SetResult(ret);
	}
	else
	{
		//ASYNCFLOW_WARN("node {0} {1}[{2}] is not waiting async callback", (void*)node, node->GetChart()->Name(), node->GetId());
		ASYNCFLOW_WARN("node {0} is not waiting async callback", (void*)node);
	}
	return true;
}

void LuaManager::GetFunc(Ref func_ref)		//  +2
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, FunctionRef);
	lua_rawgeti(L, -1, func_ref);
}

int LuaManager::ErrorFunction(lua_State* L)
{
	const char* err_msg = lua_tostring(L, -1);
	ASYNCFLOW_ERR("run node error : {0}", err_msg);
	return 0;
}


