#include "export_class_funcs.h"
#include "lua_manager.h"

using namespace asyncflow::lua;

static const char* first_arg_agent_error = "first argument type should be agent, may caused by '.' instead of ':' ";
static const char* type_string_error = "argument type string expected";

LuaAgent* GetAgent(lua_State* L)
{
	return LightUserDataWrapper::Cast<LuaAgent*>(lua_touserdata(L, 1));	
}

int asyncflow::lua::agent_attach_chart(lua_State* L)
{	
	auto* agent = GetAgent(L);
	if (agent == nullptr)
	{
		LUA_ARG_ERR(L, 1, first_arg_agent_error);
	}	
	if (!lua_isstring(L, 2))
	{
		LUA_ARG_ERR(L, 2, type_string_error);
	}
	const auto* chart_name = lua_tostring(L, 2);
	auto* manager = dynamic_cast<LuaManager*>(agent->GetManager());
	auto chart = (LuaChart*)(manager->Manager::AttachChart(agent, chart_name));
	if (chart != nullptr && lua_istable(L, 3))
	{
		lua_settop(L, 3);
		chart->SetInitTable(L);
	}
	if (chart != nullptr)
		LuaExportClass::CreateUserData(L, "chart", chart);
	else
		lua_pushnil(L);
	return 1;
}

int asyncflow::lua::agent_remove_chart(lua_State* L)
{
	auto* agent = GetAgent(L);
	if (agent == nullptr)
	{
		LUA_ARG_ERR(L, 1, first_arg_agent_error);
	}

	if (!lua_isstring(L, 2))
	{
		LUA_ARG_ERR(L, 2, type_string_error);
	}
	
	auto is_subchart = false;
	if (lua_isboolean(L, 3))
	{
		is_subchart = lua_toboolean(L, 3);
	}
	const auto* chart_name = lua_tostring(L, 2);
	auto result = agent->RemoveChart(chart_name);
	lua_pushboolean(L, result);
	return 1;
}

int asyncflow::lua::agent_get_charts(lua_State* L)
{
	auto* agent = LightUserDataWrapper::Cast<LuaAgent*>(lua_touserdata(L, 1));
	if (agent == nullptr)
	{
		LUA_ARG_ERR(L, 1, first_arg_agent_error);
	}
	bool is_subchart = false;
	if (lua_isboolean(L, 2))
	{
		is_subchart = lua_toboolean(L, 2);
	}
	const auto& chart_dict = agent->GetChartDict();
	lua_newtable(L);
	int key = 1;
	for (const auto& charts : chart_dict)
	{
		if (is_subchart)
		{
			lua_pushinteger(L, key);
			lua_pushstring(L, charts.first.c_str());
			lua_settable(L, -3);
			++key;
		}
		else
		{
			for (auto chart : charts.second)
			{
				if (chart->GetOwnerNode() == nullptr)
				{
					lua_pushinteger(L, key);
					lua_pushstring(L, charts.first.c_str());
					lua_settable(L, -3);
					++key;
					break;
				}
			}
		}
	}
	return 1;
}

int asyncflow::lua::agent_start(lua_State* L)
{
	auto* agent = GetAgent(L);
	if (agent == nullptr)
	{
		LUA_ARG_ERR(L, 1, first_arg_agent_error);
	}
	agent->Start();
	lua_pushboolean(L, 1);
	return 1;
}

int asyncflow::lua::agent_stop(lua_State* L)
{
	auto* agent = GetAgent(L);
	if (agent == nullptr)
	{
		LUA_ARG_ERR(L, 1, first_arg_agent_error);
	}
	agent->Stop();
	lua_pushboolean(L, 1);
	return 1;
}

int asyncflow::lua::agent_get_obj(lua_State* L)
{
	auto* agent = GetAgent(L);
	if (agent == nullptr)
	{
		LUA_ARG_ERR(L, 1, first_arg_agent_error);
	}
	auto* manager = dynamic_cast<LuaManager*>(agent->GetManager());
	lua_rawgeti(L, LUA_REGISTRYINDEX, manager->ObjectRef);
	lua_rawgeti(L, -1, agent->GetRefObject());
	return 1;
}

int asyncflow::lua::chart_set_callback(lua_State* L)
{
	auto chart = LightUserDataWrapper::Cast<LuaChart*>(lua_touserdata(L, 1));
	if (chart == nullptr)
	{
		LUA_ARG_ERR(L, 1, first_arg_agent_error);
	}
	if (!lua_isfunction(L, 2))
	{
		LUA_ERR(L, "argument type function expected");
	}
	auto* manager = dynamic_cast<LuaManager*>(chart->GetAgent()->GetManager());
	lua_rawgeti(L, LUA_REGISTRYINDEX, manager->FunctionRef);
	lua_pushvalue(L, 2);
	Ref ref = luaL_ref(L, -2);
	chart->SetCall(ref);
	lua_pushboolean(L, true);
	return 1;
}