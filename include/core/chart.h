#pragma once
#include "core/chart_data.h"
#include "core/node.h"
#include "debug/debug_common.h"
#include "async_event.h"

namespace asyncflow
{
	namespace core
	{
		class Chart
		{
		public:
			enum Status
			{
				Idle = 0,
				Running = 1,
			};
			Chart();
			virtual ~Chart();
			bool Init(ChartData* chart_data, Node* owner_node = nullptr);
			virtual bool InitArgs() = 0;
			virtual void SetArgs(void* args, int argc) = 0;
			virtual void ClearVariables() = 0;
			virtual void ResetVariables() = 0;

			Agent*	GetAgent() { return agent_; }
			void	SetAgent(Agent* agent) { agent_ = agent; }
			Node*	GetOwnerNode() { return owner_node_; }
			bool	IsRunning() { return status_ == Running; }
			Node*	GetNode(int id) { return node_list_[id]; }
			const std::vector<Node*>& GetNodeList() { return node_list_; }
			int     GetNodesCount() { return static_cast<int>(node_list_.size()); }
			
			void	Stop();
			
			virtual void Return(bool result);
			bool	CheckRunning();
			Status	GetStatus() { return status_; }
			void	SetStatus(Status s) { status_ = s; }
			ChartData* GetData() { return data_; }
			const std::string& Name() { return data_->Name(); }

#ifdef FLOWCHART_DEBUG
			void	StartDebug() { if(!debug_) debug_ = true; }
			void	StopDebug() { debug_ = false; ClearDebugData(); }
			bool	IsDebug() { return debug_; }
			void	SendNodeStatus(int id, const std::string& uid, Node::Status old_status, Node::Status new_status, bool result);
			virtual void SendEventStatus(std::string node_uid, const AsyncEventBase* event) = 0;

			const std::vector<debug::DebugData*>& GetDebugData() { return debug_data_list_; }
			void ClearDebugData(); //clear variable
#endif

		protected:
			Node*		owner_node_;
			ChartData*	data_;
			Agent*		agent_;
			Status		status_;
			std::vector<Node*>		node_list_;

		protected:
			bool debug_;
			std::vector<debug::DebugData*> debug_data_list_;
			int debug_data_count_;

		};		
	}
}