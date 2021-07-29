#include "export_class_funcs.h"
#include "lua_manager.h"
#include "export_global_funcs.h"

using namespace asyncflow::lua;

LuaManager* GetManager(lua_State* L)
{
	return LightUserDataWrapper::Cast<LuaManager*>(lua_touserdata(L, 1));
}

#define MANAGER_CLASS_FUNC(func_name) \
int asyncflow::lua::mgr_##func_name(lua_State* L)												\
{																								\
	auto* manager = GetManager(L);																\
	if (manager == nullptr)																		\
	{																							\
		LUA_ARG_ERR(L, 1, "please input manager!");													\
	}																							\
	LuaManager::currentManager = manager;														\
	lua_remove(L, 1);																			\
	return func_name(L);																		\
}

MANAGER_CLASS_FUNC(register_obj)
MANAGER_CLASS_FUNC(import_charts)
MANAGER_CLASS_FUNC(import_event)
MANAGER_CLASS_FUNC(set_error_handler)
MANAGER_CLASS_FUNC(step)
MANAGER_CLASS_FUNC(event)
MANAGER_CLASS_FUNC(exit)
MANAGER_CLASS_FUNC(deregister_obj)