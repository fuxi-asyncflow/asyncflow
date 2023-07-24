#pragma once
#include <list>

namespace asyncflow
{
	namespace core
	{
		class Node;

		class INodeContainer
		{
		public:
			virtual void	Push(Node* node) = 0;
			virtual Node*	Pop() = 0;
			virtual void	Remove(Node* node) = 0;
			virtual Node*	GetTop() = 0;
			virtual bool	IsEmpty() = 0;
			virtual ~INodeContainer() = default;
		};

		//The relation between nodelist and node is circular reference.
		//TODO implement NodeList as intrusive linked list
		class NodeList : public INodeContainer
		{
		private:
			std::list<Node*> node_list_;

		public:
			std::list<Node*>& GetList() { return node_list_; }
			void Push(Node* node) override;
			Node* Pop() override;
			//Make sure that the list called remove function contains the node.
			void Remove(Node* node) override;
			Node* GetTop() override;
			bool IsEmpty() override { return node_list_.empty(); }
			int Size() const { return static_cast<int>(node_list_.size()); }
			bool Contains(Node* node) const { return std::find(node_list_.begin(), node_list_.end(), node) != node_list_.end(); }
			~NodeList() override;
		};

		enum NodeResult
		{
			rFALSE = 0,
			rTRUE = 1,
			rSTOP = 2
		};
	}
}