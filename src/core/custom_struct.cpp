#include "core/custom_struct.h"
#include "core/node.h"
using namespace asyncflow::core;

void _NodeList::Push(Node* node)
{
	if (auto list = node->GetContainer())
	{
		list->Remove(node);
	}
	node_list_.push_back(node);
	node->SetContainer(this);
}

Node* _NodeList::Pop()
{
	auto node = node_list_.front();
	node_list_.pop_front();
	node->SetContainer(nullptr);
	return node;
}

Node* _NodeList::GetTop()
{
    return node_list_.front();
}


void _NodeList::Remove(Node* node)
{
	node_list_.remove(node);
	node->SetContainer(nullptr);
}

bool _NodeList::Contains(Node* node) const
{
	return node->GetContainer() == this;
}

//int NodeList::Size()
//{
//	return static_cast<int>(node_list_.size());
//}

_NodeList::~_NodeList()
{
	for (auto* node : node_list_)
	{
		node->SetContainer(nullptr);
	}
}

///////////////////////////////////////////////////////////////////

void NodeLinkedList::Push(Node* node)
{
	push_back(node->GetLink());	
}

Node* NodeLinkedList::Pop()
{
	return Node::FromLinkNode(pop_front());
}

void NodeLinkedList::Remove(Node* node)
{
	remove(node->GetLink());
}

Node* NodeLinkedList::GetTop()
{
	return Node::FromLinkNode(front());
}

bool NodeLinkedList::IsEmpty()
{
	return empty();
}

void NodeLinkedList::print()
{
	if (empty())
		printf("node list is empty\n");
	printf("==== node list begin\n");
	for(auto* cur = list_->next; cur != list_; cur = cur->next)
	{
		printf("Node: %p, prev %p, next %p\n", Node::FromLinkNode(cur), cur->prev, cur->next);
	}

	printf("==== node list end\n");
}

Node* NodeLinkedList::NodeLinkedListIterator::operator*() const
{
	return Node::FromLinkNode(cur_);	
}
