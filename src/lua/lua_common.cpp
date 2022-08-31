#include "lua_common.h"

std::string asyncflow::lua::ToString(lua_State* L, int idx)		// +0
{
	// from luaB_tostring			
	if (!luaL_callmeta(L, idx, "__tostring"))
	{
		switch (lua_type(L, idx))
		{
		case LUA_TNUMBER: {
			lua_pushvalue(L, idx);
			break;
		}
		case LUA_TSTRING:
			lua_pushvalue(L, idx);
			break;
		case LUA_TBOOLEAN:
			lua_pushstring(L, (lua_toboolean(L, idx) ? "true" : "false"));
			break;
		case LUA_TNIL:
			lua_pushliteral(L, "nil");
			break;
		default:
			lua_pushfstring(L, "%s: %p", luaL_typename(L, idx),
				lua_topointer(L, idx));
			break;
		}
	}

	auto str = lua_tolstring(L, -1, NULL); // +0
	if (str == nullptr)
		str = "-";
	lua_pop(L, 1);	// -1
	return str;
}