#pragma once
#include "core/node_attacher.h"
#include <vector>
#include <list>
#include <util/log.h>
#include "core/async_event.h"
#include "core/custom_struct.h"

namespace asyncflow
{
	namespace core
	{
		class Chart;
		class NodeData;
		class Agent;
		class NodeList;

		class Node
		{
		public:
			enum Status
			{
				Idle = 0,
				Running = 1,
				EndRun = 2
			};

			Node(Chart* chart, NodeData* data);
			Node(const Node& node) = delete;

			~Node();
			Agent*		GetAgent() const;
			Chart*		GetChart() { return chart_; }
			bool		Run();
			void		Stop();
			bool		RunFlag() { return run_flag_; }
			void		SetRunFlag(bool flag = true) { run_flag_ = flag; }
			Status		GetStatus() { return status_; }
			void		SetStatus(Status status);
			bool		GetResult() { return result_; }
			void		SetResult(bool r) { result_ = r; }
			void		SetSkip(bool skip) { skip_ = skip; }
			bool		GetSkip() { return skip_; }
			bool		IsWaitAll() { return is_wait_all; }
			void		SetWaitAllFlag(bool flag = true) { is_wait_all = flag; }
			int			GetId() { return id_; }
			NodeData*	GetData() { return data_; }
			INodeAttacher* GetAttacher() { return attacher_; }
			void		SetAttacher(INodeAttacher* attacher) { if (attacher_ != nullptr) delete attacher_; attacher_ = attacher; }
			void        SetAttacherNull() { attacher_ = nullptr; }
			bool		IsRunning() { return status_ == Running; }
			int			GetPreNodeId() { return pre_node_id_; }
			void		SetPreNodeId(int id) { pre_node_id_ = id; }
			void		SetWaitingList(NodeList* waiting_list) { waiting_list_ = waiting_list; }
			NodeList*   GetWaitingList() { return waiting_list_; }
			void        SendEventStatus(const AsyncEventBase* event);

		private:
			int				id_;
			Chart*			chart_;
			NodeData*		data_;
			bool			run_flag_;
			bool			result_;
			bool			skip_;			//not execute function in node
			bool            is_wait_all;    //waitall node flag, special handling in RunFlow function
			int				pre_node_id_;
			Status			status_;
			INodeAttacher*	attacher_;
			NodeList* waiting_list_;		
		};
	}
}
