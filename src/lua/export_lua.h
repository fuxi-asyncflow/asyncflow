#pragma once

#include "lua_common.h"
#include <string>
#include <unordered_map>

namespace asyncflow
{
	namespace lua
	{		
		class LightUserDataWrapper
		{
			void* p_;
		public:
			void SetPointer(void* p) { p_ = p; }
			template <typename T>			
			static T Cast(void* userData)
			{
#ifdef NDEBUG
#else
				if (userData == nullptr)
					return nullptr;
#endif
				return reinterpret_cast<T>(((LightUserDataWrapper*)userData)->p_);
			}
		};
		
		class LuaExportClass
		{
		public:
			LuaExportClass(lua_State* L, const std::string name)
			{
				//class_dict[name] = this;
				this->L = L;
				//luaL_newmetatable(L, name.c_str());					// +1
				lua_newtable(L);
				lua_pushliteral(L, "__index");						// +1
				lua_newtable(L);									// +1
				lua_rawset(L, -3);									// -2
				metatable_ = luaL_ref(L, LUA_REGISTRYINDEX);		// -1
				mt_dict[name] = metatable_;
			}

			void AddFunction(const std::string& func_name, lua_CFunction func)
			{
				lua_rawgeti(L, LUA_REGISTRYINDEX, metatable_);		// +1
				lua_pushliteral(L, "__index");						// +1
				lua_gettable(L, -2);								// 0				

				lua_pushstring(L, func_name.c_str());				// +1
				lua_pushcfunction(L, func);							// +1				
				lua_rawset(L, -3);									// -2				

				lua_pop(L, 2);										// -2, pop metatalbe and __index
			}

			void static CreateUserData(lua_State* L, const std::string& cls_name, void* user_data)
			{
				auto* p = lua_newuserdata(L, sizeof(LightUserDataWrapper));	// +1
				auto* wrapper = reinterpret_cast<LightUserDataWrapper*>(p);
				wrapper->SetPointer(user_data);
				//lua_pushlightuserdata(L, user_data);						// +1
				auto mt = mt_dict[cls_name];
				lua_rawgeti(L, LUA_REGISTRYINDEX, mt);				// +1
				//printf("%p\n",lua_topointer(L, -1));
				lua_setmetatable(L, -2);									// -1
			}
			
		private:
			Ref metatable_;
			lua_State* L;

			//static std::unordered_map<std::string, LuaExportClass*> class_dict;
			static std::unordered_map<std::string, Ref> mt_dict;
		};

		class LuaExportModule
		{
		public:
			LuaExportModule(const std::string& module_name)
				: module_name_(module_name)
			{
				
			}

		public:
			void AddFunction(const std::string& func_name, lua_CFunction func);
			void Export(lua_State* L);		

		private:
			std::string module_name_;
			std::unordered_map<std::string, lua_CFunction> module_funcs_;
		};
	}
}

extern "C" ASYNCFLOW_API int luaopen_asyncflow(lua_State *L);
extern "C" ASYNCFLOW_API void* get_manager();