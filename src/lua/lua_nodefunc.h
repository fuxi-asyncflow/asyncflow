#pragma once
#include "lua_common.h"
#include "core/node_func.h"

namespace asyncflow
{
	namespace lua
	{
		class LuaNodeFunc: public core::NodeFunc
		{
		public:
			bool call(core::Agent* self) override;
			static Ref CompileFunction(const std::string& code, const std::string& name);
			static NodeFunc* GetFuncFromString(const std::string& code, const std::string& name);
		private:			
			std::string code_;
			Ref func_;

		// used for test
		public:
			void SetFunc(Ref func) { func_ = func; }
		};
	}
}