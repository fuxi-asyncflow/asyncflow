#pragma once

#include "core/manager.h"
#include "core/async_event.h"

#include "lua_common.h"
#include "lua_nodefunc.h"
#include "lua_chartdata.h"
#include "lua_agent.h"
#include "lua_chart.h"

namespace asyncflow
{
	namespace lua
	{
		class LuaObjectRefHelper
		{
		public:
			static void DecRef(Ref ref) {  }
			static void IncRef(Ref ref) {  }
			static int Default() { return LUA_NOREF; }
		};

		class ConvertObjToKey
		{
		public:
			int operator()(int i);
		};

		class LuaAgent;

		using AsyncEvent = core::AsyncEvent<Ref, LuaObjectRefHelper>;

		class LuaManager : public core::Manager
		{
		public:
#ifdef BUILD_MULTITHREAD
			static thread_local LuaManager* currentManager;
#else
			static LuaManager* currentManager;
#endif

			lua_State* L;
			Ref ErrorHandler;
			Ref ObjectRef;
			Ref ChartVariableRef;
			Ref FunctionRef;

			LuaManager()
				: Manager()
				, is_init(false)
				, agent_manager_(this) {}

			core::Chart* CreateChart() override
			{
				return new  LuaChart();
			}

			core::ChartData* CreateChartData() override
			{
				return new LuaChartData();
			}

			core::AsyncEventBase* CreateAsyncEvent(int event_id, core::Agent* agent, void* args, int arg_count) override
			{
				auto* event = new AsyncEvent(event_id, agent);
				event->SetArgs((Ref*)args, arg_count);
				return event;
			}

			~LuaManager()
			{
				agent_manager_.Clear();

				luaL_unref(L, LUA_REGISTRYINDEX, ErrorHandler);
				luaL_unref(L, LUA_REGISTRYINDEX, ObjectRef);
				luaL_unref(L, LUA_REGISTRYINDEX, ChartVariableRef);
				luaL_unref(L, LUA_REGISTRYINDEX, FunctionRef);
			}

			void Init();

			Agent* RegisterGameObject(void* obj, int tick);

			bool UnregisterGameObject(void* obj);
			bool Event(int event_id, void* obj, int* args, int arg_count, bool immediate = false);

			void SetVar(lua_State* L, int var_id);
			void GetVar(lua_State* L, int var_id);
			void GetEventParam(lua_State* obj, int event_id, int param_idx);
			void Return(lua_State* L);
			BasicAgentManager& GetAgentManager() override { return agent_manager_; }
			LuaAgent* GetAgent(void* obj)
			{
				return (LuaAgent*)agent_manager_.GetAgent(obj);
			}

			std::pair<bool, std::vector<std::string>> RunScript(const char* str) override;

			bool Subchart(const std::string& chart_name, void* obj, lua_State* L, int arg_count)
			{
				auto* agent = agent_manager_.GetAgent(obj);
				//if the obj was not registered, create a new agent
				if (agent == nullptr)
				{
					this->RegisterGameObject(obj, DEFAULT_AGENT_TICK);
					agent = agent_manager_.GetAgent(obj);
				}
				return Manager::Subchart(chart_name, agent, L, arg_count);
			}

			bool StartAgent(void* obj)
			{
				auto* agent = agent_manager_.GetAgent(obj);
				if (agent == nullptr)
					return false;

				agent->Start();
				return true;
			}

			core::Chart* AttachChart(void* obj, const std::string& chart_name)
			{
				auto* agent = agent_manager_.GetAgent(obj);
				if (agent == nullptr)
				{
					ASYNCFLOW_WARN("object is not registered to asyncflow");
					return nullptr;
				}
				return Manager::AttachChart(agent, chart_name);
			}

			//agent_manager use void* get agent*
			//agent->getRef get ref, ref + lua_topointer get void*
			core::AgentManager<LuaAgent> agent_manager_;

			bool AsyncCallback(long long context, lua_State* L);		//after asynchronous call ends

			/////////////////////////////////////////////////////////////////////////////////////
			void GetFunc(Ref func_ref);

		private:
			bool is_init;
		};
	}
}