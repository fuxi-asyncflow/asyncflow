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
			virtual ~IExecutor() = default;

			virtual bool RunFlow(Node* node) = 0;
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

			bool	RunFlow(Node* node) override;
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

		template<typename InnerExecutor>
		class StackExecutor : public IExecutor
		{
		public:
			static int MAX_STACK_SIZE;
			StackExecutor():
			    stack_(MAX_STACK_SIZE)
			{
				for (auto i = 0; i < MAX_STACK_SIZE; i++)
					stack_[i] = new InnerExecutor();
				current_index_ = 0;
				current_ = stack_[0];
			}

			~StackExecutor() override
			{
				for (auto executor : stack_)
					delete executor;
			}

			bool RunFlow(Node* node) override
			{
				int old_index = current_index_;
				current_index_++;
				if (current_index_ >= MAX_STACK_SIZE)
				{
					ASYNCFLOW_ERR("executor stack over flow");
					current_index_ = old_index;
					return false;
				}
				
			    current_ = stack_[current_index_];				
				current_->RunFlow(node);

				current_index_ = old_index;
				current_ = stack_[current_index_];
				return true;
			}

			Agent* GetCurrentAgent() override { return current_->GetCurrentAgent(); }
			Node* GetCurrentNode() override { return current_->GetCurrentNode(); }
			void Remove(Node* node) override { current_->Remove(node); }

		private:
			std::vector<InnerExecutor*> stack_;
			int current_index_;
			InnerExecutor* current_;
		};

		template<typename InnerExecutor>
		int StackExecutor<InnerExecutor>::MAX_STACK_SIZE = 16;



	}
}