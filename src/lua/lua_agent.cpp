#include "lua_agent.h"
#include "lua_manager.h"

using namespace asyncflow::lua;

LuaAgent::LuaAgent(Manager* manager, void* game_object)
	: Agent(manager)
	, addr_(game_object)
{
	auto* mgr = (LuaManager*)(manager_);
	auto* L = mgr->L;
	CheckLuaStack(0);
	lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ObjectRef);		//  +1
	lua_pushvalue(L, 1);									//  +1
	obj_ = luaL_ref(L, -2);									//  -1
	lua_pop(L, 1);											//  -1
}

LuaAgent::~LuaAgent()
{
	if (obj_ != LUA_NOREF)
	{
		auto* mgr = (LuaManager*)(manager_);
		auto* L = mgr->L;
		lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ObjectRef);
		luaL_unref(L, -1, obj_);
		lua_pop(L, 1);
	}
}

void LuaAgent::RunFlow(Node* start_node)
{
	auto* const mgr = (LuaManager*)manager_;
	auto* const L = mgr->L;
	lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ErrorHandler);	//  +1
	lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ObjectRef);		//  +1
	lua_rawgeti(L, -1, obj_);								//  +1
	lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->FunctionRef);	//  +1
	manager_->GetExecutor().RunFlow(start_node);
	lua_pop(L, 4);											//  -4
}

std::string LuaAgent::GetName()
{
	auto* mgr = (LuaManager*)(manager_);
	auto* L = mgr->L;
	CheckLuaStack(0);
	lua_rawgeti(L, LUA_REGISTRYINDEX, mgr->ObjectRef);				//  +1
	lua_rawgeti(L, -1, GetRefObject());								//  +1
	if (lua_istable(L, -1) || lua_isuserdata(L, -1))
	{
		lua_getfield(L, -1, Agent::DEBUG_NAME_METHOD.c_str());		//  +1
		if (!lua_isnil(L, -1))
		{
			lua_pushvalue(L, -2);									//  +1
			if (lua_pcall(L, 1, 1, 0) == 0)							//  -1 = -2 +1
			{
				std::string str(ToString(L, -1));
				lua_pop(L, 3);										//  -3
				return str;
			}
		}
		else
		{
			lua_pop(L, 1);											//  -1
		}
	}
	std::string str(ToString(L, -1));
	lua_pop(L, 2);													//  -2
	return str;
}
