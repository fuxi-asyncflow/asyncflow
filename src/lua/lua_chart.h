#pragma once

#include "lua_common.h"
#include "core/chart.h"
#include <lua.h>

namespace asyncflow
{
	namespace lua
	{
		class LuaChart : public core::Chart
		{
		public:
			LuaChart();
			virtual ~LuaChart();
			void SetVar(lua_State* L, int idx);
			Ref	 GetVar(lua_State* L, int idx);
			void SetCall(Ref call);
			void SetInitTable(lua_State* L);
			void SetArgs(void* /*lua_State*/ L, int argc) override;
			void SetArgsFromDict(lua_State* L);
			void Return(lua_State* obj);
			void Return(bool result) override;
			void ClearVariables() override;
			void ResetVariables() override;
			bool InitArgs() override;

#ifdef FLOWCHART_DEBUG
			void SendEventStatus(std::string node_uid, const AsyncEventBase* event) override;
			void SendVariableStatus(std::string var_id, std::string old_value, std::string new_value);
#endif
			Ref		variables_;
			Ref		call_;		// return被调用，或者无可用节点时调用
			Ref		init_table_;  //varibales when attached the chart

		private:
			void InvokeCallback(lua_State* L);
		};
	}
}
