#pragma once
#include <list>
#include "util/list.h"

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
		class _NodeList : public INodeContainer
		{
		private:
			std::list<Node*> node_list_;

		public:			
			std::list<Node*>::iterator begin()  { return node_list_.begin(); }
			std::list<Node*>::iterator end() { return node_list_.end(); }
			void Push(Node* node) override;
			Node* Pop() override;
			//Make sure that the list called remove function contains the node.
			void Remove(Node* node) override;
			Node* GetTop() override;
			bool IsEmpty() override { return node_list_.empty(); }
			int Size() const { return static_cast<int>(node_list_.size()); }
			bool Contains(Node* node) const;
			~_NodeList() override;
		};


		class NodeLinkedListIterator;
		class NodeLinkedList : public LinkedList, public INodeContainer
		{
			class NodeLinkedListIterator
			{
			public:
				NodeLinkedListIterator(LinkedNode* node)
					: cur_(node){}
				NodeLinkedListIterator(const NodeLinkedListIterator& v) = default;
				NodeLinkedListIterator(NodeLinkedListIterator&& v) = default;
				NodeLinkedListIterator& operator=(const NodeLinkedListIterator& v) = default;

			private:
				LinkedNode* cur_;
			public:
				Node* operator* () const;

				NodeLinkedListIterator& operator++()
				{
					cur_ = cur_->next;
					return *this;
				}

				NodeLinkedListIterator operator++(int)
				{
					auto it(*this);
					++(*this);
					return it;
				}

				NodeLinkedListIterator& operator--()
				{
					cur_ = cur_->prev;
					return *this;
				}

				NodeLinkedListIterator operator--(int)
				{
					auto it(*this);
					--(*this);
					return it;
				}

				friend bool operator==(NodeLinkedListIterator lhs, NodeLinkedListIterator rhs) noexcept
				{
					return lhs.cur_ == rhs.cur_;
				}

				friend bool operator!=(NodeLinkedListIterator lhs, NodeLinkedListIterator rhs) noexcept
				{
					return !(lhs == rhs);
				}

				friend class NodeLinkedList;
			};
			using TIt= NodeLinkedListIterator;
		public:
			~NodeLinkedList() override;
			void	Push(Node* node) override;
			Node*	Pop() override;
			void	Remove(Node* node) override;
			Node*	GetTop() override;
			bool	IsEmpty() override;
			TIt		Erase(const TIt& it);
			TIt begin() const { return TIt(list_->next); }
			TIt end() const { return TIt(list_); }

			void print();
		};

		enum NodeResult
		{
			rFALSE = 0,
			rTRUE = 1,
			rSTOP = 2
		};
	}
}
