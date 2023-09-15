#pragma once
#include <vector>
#include "util/log.h"
#include "core/custom_struct.h"

namespace asyncflow
{
	namespace core
	{		
		class Agent;

		class IExecutor
		{
		public:
			IExecutor() = default;
			virtual ~IExecutor() = default;

			// be careful, this func should only be called inside agent's RunFlow function
			// because LuaAgent need to prepare some data on stack 
			virtual bool RunFlow(Node* node) = 0;
			virtual Agent* GetCurrentAgent() = 0;
			virtual Node* GetCurrentNode() = 0;
			//virtual void Remove(Node* node) = 0;
		};

		class DfsExecutor : public IExecutor, INodeContainer
		{
		public:
			static int MAX_NODES_PER_EXECUTION;
			DfsExecutor();

			Node* GetTop() override	{ return node_list_.back();}

			Node* Pop() override;

            void Push(Node* node) override;

            void Remove(Node* node) override;

            bool IsEmpty() override { return node_list_.empty(); }

			bool	RunFlow(Node* node) override;
			Agent* GetCurrentAgent() override { return current_agent_; }
			Node* GetCurrentNode() override { return current_node_; }

		private:
			std::vector<Node*> node_list_;
			std::vector<Node*> history_nodes_;
			Agent* current_agent_;
			Node* current_node_;

		protected:
			void AddSubsequenceNodes(Node* node);
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
			void Remove(Node* node) { current_->Remove(node); }

		private:
			std::vector<InnerExecutor*> stack_;
			int current_index_;
			InnerExecutor* current_;
		};

		template<typename InnerExecutor>
		int StackExecutor<InnerExecutor>::MAX_STACK_SIZE = 16;
	}
}