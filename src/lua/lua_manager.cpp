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
	FunctionRef = InitFuncRef(L);		// 0
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
	if (current_event_ == nullptr)
	{
		// pause
		ASYNCFLOW_WARN("current_event is null, get event param must right after event node");
		lua_pushnil(L);
		return;
	}

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

bool LuaManager::ContinueAsyncNode(long long context, lua_State* L)
{
	CheckLuaStack(0);
	const auto result = Manager::ContinueAsyncNode(context);
	if (result)
	{
		auto* node = reinterpret_cast<Node*>(context);		
		auto const var_id = node->GetData()->GetVarId();
		if (var_id >= 0)
			((LuaChart*)node->GetChart())->SetVar(L, var_id);
		bool ret = lua_toboolean(L, -1);
		node->SetResult(ret);
	}	
	return result;
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

Ref LuaManager::InitFuncRef(lua_State* L)
{
	lua_getglobal(L, "asyncflow");				// +1
	lua_getfield(L, -1, "func_ref");		// +1
	if(lua_isnumber(L, -1))
	{
		auto ref = static_cast<Ref>(lua_tointeger(L, -1));
		lua_pop(L, 2);
		return ref;
	}

	lua_newtable(L);									//  +1
	auto ref = luaL_ref(L, LUA_REGISTRYINDEX);		//  -1
	lua_pushinteger(L, ref);							// +1
	lua_setfield(L, -3, "func_ref");				// -1
	lua_pop(L, 2);
	return ref;	
}

bool LuaManager::RunFlow(Node* node)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, ErrorHandler);						//  +1
	lua_rawgeti(L, LUA_REGISTRYINDEX, ObjectRef);							//  +1
	lua_rawgeti(L, -1, ((LuaAgent*)(node->GetAgent()))->GetRefObject());	//  +1
	lua_rawgeti(L, LUA_REGISTRYINDEX, FunctionRef);							//  +1
	auto ret = Manager::RunFlow(node);
	lua_pop(L, 4);																	//  -4
	return ret;
}


