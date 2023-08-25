#pragma once
#include "core/node_attacher.h"
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
			NodeResult	Run();
			void		Stop();
			bool		RunFlag() { return run_flag_; }
			void		SetRunFlag(bool flag = true) { run_flag_ = flag; }
			Status		GetStatus() const { return status_; }
			void		SetStatus(Status status);
			NodeResult	GetResult() { return result_; }
			void		SetResult(NodeResult r) { result_ = r; }
			void		SetResult(bool b) { result_ = b ? rTRUE : rFALSE; }
			void		SetSkip(bool skip) { skip_ = skip; }
			bool		GetSkip() { return skip_; }
			bool		IsWaitAll() { return is_wait_all; }
			void		SetWaitAllFlag(bool flag = true) { is_wait_all = flag; }
			int			GetId() { return id_; }
			NodeData*	GetData() const { return data_; }
			INodeAttacher* GetAttacher() { return attacher_; }
			void		SetAttacher(INodeAttacher* attacher) { if (attacher_ != nullptr) delete attacher_; attacher_ = attacher; }
			void		SetAttacherNull() { attacher_ = nullptr; }
			bool		IsRunning() const { return status_ == Running; }
			int			GetPreNodeId() { return pre_node_id_; }
			void		SetPreNodeId(int id) { pre_node_id_ = id; }
			void		SetContainer(INodeContainer* container) { container_ = container; }
			INodeContainer*	GetContainer() { return container_; }
			LinkedNode* GetLink() { return &link_; }
			void		SendEventStatus(const AsyncEventBase* event);

			void		IncTrueCount()  { true_count_++; }
			void		IncFalseCount() { false_count_++; }
			int			GetTrueCount() const { return true_count_; }
			int			GetFalseCount() const { return false_count_; }

			static Node* FromLinkNode(LinkedNode* link);

		private:
			int				id_;
			int				pre_node_id_;
			int				true_count_;
			int				false_count_;
			LinkedNode		link_;
			Chart*			chart_;
			NodeData*		data_;
			NodeResult		result_;
			Status			status_;
			bool			run_flag_;
			bool			skip_;			//not execute function in node
			bool			is_wait_all;    //waitall node flag, special handling in RunFlow function
			
			INodeAttacher*	attacher_;
			INodeContainer* container_;
		};
	}
}
