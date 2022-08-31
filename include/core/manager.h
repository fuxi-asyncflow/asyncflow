#pragma once

#include <deque>
#include <string>
#include <vector>

#include "core/agent_manager.h"
#include "core/async_event.h"
#include "core/chart.h"

#include "core/event_manager.h"
#include "core/timer_manager.h"
#include "core/async_manager.h"
#include "debug/debug_common.h"
#include "debug/websocket_manager.h"
#include "core/data_manager.h"
#include "core/executor.h"
#include "core/event_queue.h"
#include "core/dying_agents.h"

namespace asyncflow
{
	namespace core
	{
		class Manager
		{
		public:
			Manager();
			virtual ~Manager();
			void	Step(int milliseconds = 10);
			bool	Event(int event_id, Agent* obj, void* args, int arg_count, bool immediate = false);
			int		ImportFile(const std::string& file_name);
			int		ImportJson(const std::string& json_str);
			int		ImportChatData(const std::vector<ChartData*>& data_list);
			std::vector<ChartData*> ParseChartsFromJson(const std::string& json_str);
			int		ImportEvent(const std::string& file_name);
			bool	ReloadChartData(ChartData* new_data) const;
			bool	UnregisterGameObject(Agent* agent);

			Chart* AttachChart(Agent* agent, const std::string& chart_name);
			void RestartChart(const std::string& chart_name);

			TimerManager& GetTimerManager() { return timer_manager_; }
			EventManager& GetEventManager() { return eventManager; }
			AsyncManager& GetAsyncManager() { return async_manager_; }
			virtual BasicAgentManager& GetAgentManager() = 0;
			IExecutor& GetExecutor() { return executor_; }

			Agent* GetCurrentAgent() { return executor_.GetCurrentAgent(); }
			Node* GetCurrentNode() { return executor_.GetCurrentNode(); }
			int64_t Now() { return GetTimerManager().Now(); }

		public:
			// inside node function
			void Wait(int milliseconds);
			bool WaitAll(const std::vector<int>& args);
			bool StopNode(const std::vector<int>& args);
			bool StopFlow(const std::vector<int>& args);
			bool WaitEvent(Agent* agent, int event_id);
			bool Subchart(const std::string& chart_name, Agent* agent, void* args, int arg_count);
			int64_t	 CreateAsyncContext();	//Save current context

			virtual Chart* CreateChart() = 0;
			virtual ChartData* CreateChartData() = 0;
			virtual AsyncEventBase* CreateAsyncEvent(int event_id, core::Agent* agent, void* args, int arg_count) = 0;

		public:
			virtual std::pair<bool, std::vector<std::string>> RunScript(const char* str) = 0;   //run script
			ChartData* GetChartData(const std::string& chart_name);

		protected:
			int64_t			current_frame_;
			AsyncEventBase* current_event_;

			TimerManager	timer_manager_;
			AsyncManager	async_manager_;

			DfsExecutor		executor_;
			EventQueue		event_queue_;
			DyingAgents		dying_agents_;

			std::unordered_map<std::string, ChartData*>* chart_data_dict_;

			bool in_step_;				// flag for inside or outside step function 

		private:
			void HandleEvent(AsyncEventBase& event);

		protected:
			
			static EventManager	eventManager;
			int default_time_interval_;
			bool immediate_subchart_;

		public:
			const static int DEFAULT_AGENT_TICK;
			bool AUTO_REGISTER;
			static DataManager dataManager;
			bool IsImmediateSub() { return immediate_subchart_; }
			void SetImmediateSub(bool flag) { immediate_subchart_ = flag; }
			void SetDefaulTimeInterval(int interval) { default_time_interval_ = interval; }

#ifdef FLOWCHART_DEBUG
		public:
			debug::WebsocketManager& GetWebsocketManager() { return websocket_manager_; }
			std::vector<debug::ChartInfo*> GetDebugChartList(const std::string& object_name, const std::string& chart_name);
			std::vector<Chart*> GetChartsByName(const std::string& chart_name);
			bool SetBreakpoint(NodeData* data);
			bool DeleteBreakpoint(NodeData* data);		
		protected:
			debug::WebsocketManager websocket_manager_;
#endif
		};
	}
}