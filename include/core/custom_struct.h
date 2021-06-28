#pragma once
#include "core/node.h"

namespace asyncflow
{
	namespace core
	{
		//The relation between nodelist and node is circular reference.
		class NodeList
		{
		private:
			std::list<Node*> node_list_;

		public:
			std::list<Node*>& GetList() { return node_list_; }
			void PushBack(Node* node);
			Node* PopFront();
			//Make sure that the list called remove function contains the node.
			void Remove(Node* node);
			int Size();
			~NodeList();
		};
	}
}