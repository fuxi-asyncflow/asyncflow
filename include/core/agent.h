#pragma once
#include <unordered_map>
#include <vector>
#include "core/chart.h"
#include "core/timer_manager.h"
#include "custom_struct.h"


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
			using NodeList = NodeLinkedList;
			using AgentID = uint64_t;

			enum Status
			{
				Running = 1,
				Destroying = 2
			};

			Agent(Manager* manager);
			virtual ~Agent();

			int Start();
			void Stop();		

			Chart*		AttachChart(ChartData* chart_data);
			bool		AddChart(Chart* chart, Node* node = nullptr);
			bool        StartChart(const std::string& chart_name);
			bool		StartChart(Chart* chart, bool sync, void* args = nullptr, int argc = 0);
			bool		EraseChart(Chart* chart);
			bool		RemoveChart(const std::string& chart_name);
			bool		StopChart(const std::string& chart_name);

			AgentID		GetId() const { return id_; }
			void		SetId(AgentID id) { id_ = id; }
			virtual std::string GetName() { return ""; }
			Manager*	GetManager() { return manager_; }
			bool		IsRunning() { return status_ == Running; }
			int			GetTickInterval() { return tick_interval_; }
			void		SetTickInterval(int tick);
			void		WaitEvent(Node* node, int event_id);			
			Chart*		FindChart(const std::string& chart_name, Node* owner_node);
			const std::unordered_map<std::string, std::vector<Chart*>>& GetChartDict() { return chart_dict_; }
			Status		GetStatus() { return status_; }
			void		SetStatus(Status s) { status_ = s; }
			NodeList*	GetWaitNodes(int event_id, bool create = true);
			

		protected:
			AgentID			id_;
			int				tick_interval_;				// tick event interval for agent
			Status			status_;
			Manager*		manager_;
			AgentTimer		timer_;
			std::unordered_map<std::string, std::vector<Chart*>> chart_dict_;
			std::unordered_map<int, NodeList*> waiting_nodes_dict_;
		};


	}
}
