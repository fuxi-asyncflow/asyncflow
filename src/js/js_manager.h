#pragma once
#include "core/manager.h"
#include "js_agent.h"
#include "js_chartdata.h"
#include "js_chart.h"
#ifdef FLOWCHART_DEBUG
#include "js_debugger.h"
#endif

namespace asyncflow
{
	namespace js 
	{
		class JsObjectRefHelper
		{
		public:
			static void DecRef(int ref) {  }
			static void IncRef(int ref) {  }
			static int Default() { return 0; }
		};

		using AsyncEvent = core::AsyncEvent<int, JsObjectRefHelper>;

		class JsManager : public core::Manager
		{
		public:
			JsManager():
				Manager(),
				agent_manager_(this)
			{

			}

			core::Chart* CreateChart() override
			{
				return new JsChart();
			}

			core::ChartData* CreateChartData() override
			{
				return new JsChartData();
			}

			core::AsyncEventBase* CreateAsyncEvent(int event_id, core::Agent* agent, void* args, int arg_count) override
			{
				auto* event = new AsyncEvent(event_id, agent);
				event->SetArgs((int *)args, arg_count);
				return event;
			}

			BasicAgentManager& GetAgentManager() override { return agent_manager_; }
			std::pair<bool, std::vector<std::string>> RunScript(const char* str) override {
				return std::pair<bool, std::vector<std::string>>();
			}

			Agent* RegisterGameObject(int obj, int tick);

			core::Chart* AttachChart(int obj, const std::string& chart_name);

			JsAgent* GetAgent(int id)
			{
				return (JsAgent*)agent_manager_.GetAgent(id);
			}

			int GetVar(int idx)
			{
				auto chart = static_cast<JsChart*>(GetCurrentNode()->GetChart());
				int id = chart->GetVar(idx);
				return id;
			}

			void SetVar(int idx, int id)
			{
				auto chart = static_cast<JsChart*>(GetCurrentNode()->GetChart());
				chart->SetVar(idx, id);
			}

			bool Event(int event_id, int obj_id, int* args, int arg_count, bool immediate = false);
			int GetEventPatam(int event_id, int param_idx);

			void Return(int id);
			core::AgentManager<JsAgent> agent_manager_;

#ifdef FLOWCHART_DEBUG
			debug::JsDebugger& GetJsDebugger() { return js_debugger_; }

			void Step(int milliseconds)
			{
				Manager::Step(milliseconds);
				js_debugger_.Step();
			}

			void OnMessage(char* str)
			{
				rapidjson::Document doc;
				bool valid_json = util::JsonUtil::ParseJson(str, doc);
				debug::Debugger::HandleMessage(str, this, &js_debugger_);
			}

		private:
			debug::JsDebugger js_debugger_;
#endif
		};
	}
}
