#include "lua_nodefunc.h"
#include "lua_agent.h"
#include "lua_manager.h"

using namespace asyncflow::core;
using namespace asyncflow::lua;


bool LuaNodeFunc::call(Agent* agent)
{
	auto* const mgr = (LuaManager*)(agent->GetManager());
	auto* const L = mgr->L;
	bool  result = false;
	CheckLuaStack(0);
	
	lua_rawgeti(L, -1, func_);
	lua_pushvalue(L, -3);

	//TODO set error handle func
	if (lua_pcall(L, 1, 1, -6))
	{
		auto current_chart = (LuaChart*)mgr->GetCurrentNode()->GetChart();
		auto node_data = mgr->GetCurrentNode()->GetData();
		ASYNCFLOW_ERR("run node[{0}] {1} error in chart {2}", node_data->GetId(), node_data->GetUid(), current_chart->Name());
		Chart* temp_chart = current_chart;
		int chart_stack = 0;
		int node_id = mgr->GetCurrentNode()->GetId();
		while (true)
		{
			ASYNCFLOW_ERR("chart stack[{0}] {1}[{2}]", chart_stack, temp_chart->Name(), node_id);
			for (int var_id = 0; var_id < temp_chart->GetData()->GetVarCount(); var_id++)
			{
				static_cast<LuaChart*>(temp_chart)->GetVar(L, var_id);
				std::string var = ToString(L, -1);
				ASYNCFLOW_ERR("va[{0}]: ${1} = {2}.", var_id, temp_chart->GetData()->GetVariableName(var_id), var);
				lua_pop(L, 1);
			}
			if (temp_chart->GetOwnerNode() == nullptr || chart_stack > 32)
				break;
			node_id = temp_chart->GetOwnerNode()->GetId();
			temp_chart = temp_chart->GetOwnerNode()->GetChart();
			chart_stack++;

		}
		result = false;
	}
	else
	{
		if (lua_type(L, -1) == LUA_TNUMBER)
		{
			result = ((int)lua_tonumber(L, -1)) != 0;
		}
		else
			result = lua_toboolean(L, -1);
	}
	lua_pop(L, 1);
	return result;
}

Ref LuaNodeFunc::GetFunctionByName(const std::string& name)
{
	auto L = LuaManager::currentManager->L;
	CheckLuaStack(0);
	lua_getglobal(L, "asyncflow");		//  +1
	assert(!lua_isnil(L, -1));
	lua_getfield(L, -1, "node_funcs");	//  +1
	assert(!lua_isnil(L, -1));
	lua_getfield(L, -1, name.c_str());	//  +1
	if (lua_isnil(L, -1))
	{
		ASYNCFLOW_ERR("cannot find node function {0}", name);
		lua_pop(L, 3);
		return LUA_NOREF;
	}

	lua_rawgeti(L, LUA_REGISTRYINDEX, LuaManager::currentManager->FunctionRef);   //  +1
	lua_pushvalue(L, -2);           //  +1
	auto ref = luaL_ref(L, -2);	    //  -1
	lua_pop(L, 4);					//  -4
	return ref;
}


NodeFunc* LuaNodeFunc::GetFuncFromString(const std::string& code, const std::string& name)
{
	auto L = LuaManager::currentManager->L;
	if(code.empty())
	{
		auto const func = GetFunctionByName(name);
		if (func == LUA_NOREF)
			return nullptr;
		auto* f = new LuaNodeFunc;
		f->func_ = func;
		return f;
	}

	luaL_dostring(L, code.c_str());
	return nullptr;
}