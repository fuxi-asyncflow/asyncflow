#pragma once
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stack>
#include <list>
#include "core/chart.h"
#include "core/timer_manager.h"
#include "custom_struct.h"
#include "executor.h"

namespace asyncflow
{
	namespace core
	{
		class Agent;
		class AsyncEventBase;
		class Manager;

		class Agent
		{
		public:
			static std::string DEBUG_NAME_METHOD;
			static const int AGENT_INIT_ID = 1;

			enum Status
			{
				Running = 1,
				Destroying = 2
			};

			Agent(Manager* manager);
			virtual ~Agent();

			void Start();
			void Stop();		

			Chart*		AttachChart(ChartData* chart_data);
			bool		AddChart(Chart* chart, Node* node = nullptr);
			bool        StartChart(const std::string& chart_name);
			bool		StartChart(Chart* chart, bool is_async = true);
			bool		EraseChart(Chart* chart);
			bool		RemoveChart(const std::string& chart_name);
			bool		StopChart(const std::string& chart_name);
			void		HandleEvent(const AsyncEventBase& event);
			virtual void		RunFlow(Node* start_node);

			int			GetId() { return id_; }
			void		SetId(int id) { id_ = id; }
			virtual std::string GetName() { return ""; }
			Manager*	GetManager() { return manager_; }
			bool		IsRunning() { return status_ == Running; }
			int			GetTickInterval() { return tick_interval_; }
			void		SetTickInterval(int tick);
			void		WaitEvent(Node* node, int event_id);			
			Chart*		FindChart(const std::string& chart_name, Node* owner_node);
			std::vector<std::string> GetRunningChartNames();
			const std::unordered_map<std::string, std::vector<Chart*>>& GetChartDict() { return chart_dict_; }
			Status		GetStatus() { return status_; }
			void		SetStatus(Status s) { status_ = s; }
			

		protected:
			int				id_;
			int				tick_interval_;				// tick event interval for agent
			Status			status_;
			Manager*		manager_;
			AgentTimer		timer_;
			std::unordered_map<std::string, std::vector<Chart*>> chart_dict_;
			std::vector<NodeList*> waiting_nodes_list;
			

		private:			
			NodeList* GetWaitNodes(int event_id);
		};


	}
}
