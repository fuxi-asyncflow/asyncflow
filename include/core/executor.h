#pragma once
#include "core/node.h"

namespace asyncflow
{
	namespace core
	{
		class IExecutor
		{
		public:
			IExecutor() :
				loop_check_(true) {}

			virtual void RunFlow(Node* node) = 0;
			virtual Agent* GetCurrentAgent() = 0;
			virtual Node* GetCurrentNode() = 0;
			virtual void Remove(Node* node) = 0;
			void SetLoopCheck(bool flag) { loop_check_ = flag; }

		protected:
			bool loop_check_;
		};

		class DfsExecutor : public IExecutor
		{
		public:
			DfsExecutor();

			Node* PopNode()
			{
				auto* next = node_list_.back();
				history_nodes_.push_back(next);
				node_list_.pop_back();
				return next;
			}

			void Remove(Node* node) override
			{
				node_list_.erase(std::remove(node_list_.begin(), node_list_.end(), node), node_list_.end());
			}

			void	RunFlow(Node* node) override;
			Agent* GetCurrentAgent() override { return current_agent_; }
			Node* GetCurrentNode() override { return current_node_; }

		private:
			std::vector<Node*> node_list_;
			std::vector<Node*> history_nodes_;
			Agent* current_agent_;
			Node* current_node_;

		protected:
			void AddSubsequenceNodes(Node* node, NodeResult result);
		};
	}
}