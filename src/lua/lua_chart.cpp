#include "lua_chart.h"
#include "lua_manager.h"

using namespace asyncflow::core;
using namespace asyncflow::lua;

///////////////////////////////////////////////////////////////////////////////////////////
LuaChart::LuaChart()
	: call_(LUA_NOREF), variables_(LUA_NOREF), init_table_(LUA_NOREF)
{
}

LuaChart::~LuaChart()
{
	ClearVariables();
	auto mgr = dynamic_cast<LuaManager*>(agent_->GetManager());
	if (call_ != LUA_NOREF)
	{
		auto L = mgr->L;
		lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->FunctionRef);
		luaL_unref(L, -1, call_);
		call_ = LUA_NOREF;
		lua_pop(L, 1);
	}
	if (init_table_ != LUA_NOREF)
	{
		auto L = mgr->L;
		lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ChartVariableRef);
		luaL_unref(L, -1, init_table_);
		init_table_ = LUA_NOREF;
		lua_pop(L, 1);
	}
}

int LuaChart::GetVar(lua_State* L, int idx) // [+0, +1, +1]
{
	++idx;	//lua table idx start from 1;
	CheckLuaStack(1);
	if (idx > data_->GetVarCount())
	{
		ASYNCFLOW_WARN("var id {0} is out of range in chart {1}", idx, Name());
		lua_pushnil(L);
		return 1;
	}
	auto mgr = dynamic_cast<LuaManager*>(agent_->GetManager());
	lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ChartVariableRef);    //+1
	lua_rawgeti(L, -1, variables_);                                     //+1
	lua_rawgeti(L, -1, idx);	                                        //+1
	lua_remove(L, -2);                                                  //-1
	lua_remove(L, -2);                                                  //-1
	return 1;
}

void LuaChart::InvokeCallback(lua_State* L)	// [+0, +0, -]
{
	//return value locates on the top of the stack
	if (call_ == LUA_NOREF)
		return;
	CheckLuaStack(0);
	int top = lua_gettop(L);
	lua_pushvalue(L, -1);
	auto mgr = dynamic_cast<LuaManager*>(agent_->GetManager());
	lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->FunctionRef);
	lua_rawgeti(L, -1, call_);
	luaL_unref(L, -2, call_);
	call_ = LUA_NOREF;
	//result, function_table, call_, top
	if (lua_isfunction(L, -1))
	{
		lua_pushvalue(L, -3);
		//result, function_table, call, result, top 
		if (lua_pcall(L, 1, 0, mgr->ErrorHandler))
		{
			// result, function_table, call, error_msg, top
			const char* err_msg = lua_tostring(L, -1);
		}
	}
	lua_settop(L, top);
}

void LuaChart::Return(lua_State* L) // [-1, +0, -]
{
	//TODO Because some nodes have added to the handling_nodes_list before chart was stopped,
	//So some nodes may continue to run after stopping the chart
	if (status_ != Running)
		return;
	bool result = lua_toboolean(L, -1);
	auto owner_node = this->GetOwnerNode();
	if (owner_node != nullptr)
	{
		auto var_id = owner_node->GetData()->GetVarId();
		if (var_id >= 0)
			((LuaChart*)(owner_node->GetChart()))->SetVar(L, var_id);
	}

	Chart::Return(result);
	InvokeCallback(L);
}

void LuaChart::Return(bool result)
{
	Chart::Return(true);
	auto L = dynamic_cast<LuaManager*>(agent_->GetManager())->L;
	lua_pushboolean(L , 1);		//push true as default callback function parameter
	InvokeCallback(L);
}

void LuaChart::SetCall(Ref call)
{
	if (call_ != LUA_NOREF)
	{
		auto mgr = dynamic_cast<LuaManager*>(agent_->GetManager());
		auto L = mgr->L;
		lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->FunctionRef);
		luaL_unref(L, -1, call_);
		call_ = LUA_NOREF;
		lua_pop(L, 1);
	}
	call_ = call;
}

void LuaChart::SetInitTable(lua_State* L)
{
	CheckLuaStack(0);
	auto mgr = dynamic_cast<LuaManager*>(agent_->GetManager());
	if (init_table_ != LUA_NOREF)
	{
		auto L = mgr->L;
		lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ChartVariableRef);
		luaL_unref(L, -1, init_table_);
		init_table_ = LUA_NOREF;
		lua_pop(L, 1);
	}
	SetArgsFromDict(L);
	lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ChartVariableRef);	// +1		
	lua_pushvalue(L, -2);																// +1
	init_table_ = luaL_ref(L, -2);														// -1
	lua_pop(L, 2);																		// -1
}

// Lua table starts from 1, so does index.
void LuaChart::SetVar(lua_State* L, int idx)     //[-0, +0, -]
{
	++idx;	//lua table idx start from 1;
	CheckLuaStack(0);
	// 1  - value, 2- weak
	if (idx > data_->GetVarCount())
	{
		ASYNCFLOW_WARN("var id {0} is out of range in chart {1}", idx, Name());
		return;
	}
	auto mgr = dynamic_cast<LuaManager*>(agent_->GetManager());
	lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ChartVariableRef);    //+1
	lua_rawgeti(L, -1, variables_);                                     //+1
#ifdef FLOWCHART_DEBUG
	lua_rawgeti(L, -1, idx);                                            //+1
	auto old_value = ToString(L, -1);
	lua_pop(L, 1);                                                      //-1
	lua_pushvalue(L, 1);                                               //+1
	auto new_value = ToString(L, -1);
	int pos = idx - 1;  //Lua index start from 1, need to convert to vector index;
	SendVariableStatus(data_->GetVariableName(pos), old_value, new_value);
#else
	lua_pushvalue(L, 1);
#endif
	lua_rawseti(L, -2, idx);                                            //-1
	lua_pop(L, 2);                                                      //-2
}

void LuaChart::ClearVariables()    //[-0, +0, -]
{
	if (variables_ != LUA_NOREF)
	{
		auto mgr = dynamic_cast<LuaManager*>(agent_->GetManager());
		auto L = mgr->L;
		lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ChartVariableRef);
		luaL_unref(L, -1, variables_);
		variables_ = LUA_NOREF;
		lua_pop(L, 1);
	}
}

void LuaChart::ResetVariables()
{
	ClearVariables();
	InitArgs();                                                  // -1
}

// argc is the count of the input args.
void LuaChart::SetArgs(void* p, int argc)      // [-0, +0, -]
{
	lua_State* L = reinterpret_cast<lua_State*>(p);
	auto mgr = dynamic_cast<LuaManager*>(agent_->GetManager());
	int start = lua_gettop(L) - argc;
	lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ChartVariableRef);
	lua_rawgeti(L, -1, variables_);
	int data_argc = data_->GetParamCount();
	for (int i = 1; i <= argc; i++)
	{
		if (i > data_argc)    //the count of args overload the amount of the params of the chart
			break;

		lua_pushvalue(L, start + i);
		lua_rawseti(L, -2, i);

	}
	lua_pop(L, 2);
}

bool LuaChart::InitArgs()     // [-0, +0, -]
{
	auto var_count = data_->GetVarCount();
	auto mgr = dynamic_cast<LuaManager*>(agent_->GetManager());
	auto L = mgr->L;
	CheckLuaStack(0);
	lua_newtable(L);												// +1
	auto param_count = ((LuaChartData*)this->data_)->GetParamCount();
	if (init_table_ != LUA_NOREF)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ChartVariableRef);	// +1
		lua_rawgeti(L, -1, init_table_);							// +1
		for (int i = 1; i <= param_count; i++)
		{
			lua_rawgeti(L, -1, i);									// +1
			lua_rawseti(L, -4, i);									// -1
		}
		for (int i = param_count + 1; i <= var_count; i++)
		{
			lua_pushnil(L);											// +1
			lua_rawseti(L, -4, i);									// -1
		}
		lua_pop(L, 2);												// -2
	}
	else
	{
		for (int i = 1; i <= var_count; i++)
		{
			lua_pushnil(L);											// +1
			lua_rawseti(L, -2, i);									// -1
		}
	}
	lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ChartVariableRef);		// +1
	lua_pushvalue(L, -2);											// +1
	variables_ = luaL_ref(L, -2);									// -1
	lua_pop(L, 2);													// -2
	return true;
}

//table locates on the top of stack
void LuaChart::SetArgsFromDict(lua_State* L)
{
	CheckLuaStack(1);
	lua_pushvalue(L, LUA_REGISTRYINDEX);							// +1
	lua_pushnil(L);													// +1
	std::map<std::string, Ref> dict;
	const auto& param_info = ((LuaChartData*)this->data_)->GetVariableInfo();
	auto param_count = ((LuaChartData*)this->data_)->GetParamCount();
	while (lua_next(L, -3))											// -1 +2|0
	{
		//temporarily store the variables in LUA_REGISTRYINDEX table
		std::string name;
		auto key_type = lua_type(L, -2);
		if (key_type == LUA_TSTRING)
		{
			name = lua_tostring(L, -2);
		}
		else if (key_type == LUA_TNUMBER)
		{
			int idx = (int)lua_tonumber(L, -2) - 1;
			if (idx >= 0 && idx < param_count)
			{
				name = param_info[idx].name;
			}
			else
			{
				lua_pop(L, 1);										// -1
				continue;
			}
		}
		else
		{
			lua_pop(L, 1);											// -1
			continue;
		}
		Ref ref = luaL_ref(L, -3);									// -1
		dict[name] = ref;
	}
	lua_pop(L, 1);													// -1
	lua_newtable(L);												// +1
	for (auto i = 0; i < param_count; ++i)
	{
		auto& param = param_info[i];
		auto it = dict.find(param.name);
		if (it != dict.end())
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, it->second);
			lua_rawseti(L, -2, i + 1);
		}
		else
		{
			lua_pushnil(L);
			lua_rawseti(L, -2, i + 1);
		}
	}
	//clear variables temporarily store in LUA_REGISTRYINDEX table
	for (auto& pair : dict)
	{
		luaL_unref(L, LUA_REGISTRYINDEX, pair.second);
	}
}

#ifdef FLOWCHART_DEBUG
void LuaChart::SendEventStatus(std::string node_uid, const AsyncEventBase* event)
{
	auto* lua_event = (AsyncEvent*)event;
	std::vector<std::string> args;
	auto mgr = dynamic_cast<LuaManager*>(agent_->GetManager());
	auto L = mgr->L;
	for (int i = 0; i < lua_event->argCount(); i++)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, lua_event->GetEventParam(i));
		args.push_back(ToString(L, -1));
		lua_pop(L, 1);
	}
	auto& event_name = agent_->GetManager()->GetEventManager().GetEventName(lua_event->Id());
	debug::EventStatusData* data = new debug::EventStatusData(node_uid, event_name, args, lua_event->argCount());
	data->id = debug_data_count_++;
	debug_data_list_.push_back(data);
}

void LuaChart::SendVariableStatus(std::string var_name, std::string old_value, std::string new_value)
{
	if (!debug_)
		return;
	if (old_value != new_value)
	{
		auto current_uid = agent_->GetManager()->GetCurrentNode()->GetData()->GetUid();
		debug::VariableStatusData* data = new debug::VariableStatusData(var_name, current_uid, old_value, new_value);
		data->id = debug_data_count_++;
		debug_data_list_.push_back(data);
	}
}
#endif
