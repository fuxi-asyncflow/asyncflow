#include "core/custom_struct.h"
#include "core/node.h"
using namespace asyncflow::core;

void NodeList::Push(Node* node)
{
	if (auto list = node->GetContainer())
	{
		list->Remove(node);
	}
	node_list_.push_back(node);
	node->SetContainer(this);
}

Node* NodeList::Pop()
{
	auto node = node_list_.front();
	node_list_.pop_front();
	node->SetContainer(nullptr);
	return node;
}

Node* NodeList::GetTop()
{
    return node_list_.front();
}


void NodeList::Remove(Node* node)
{
	node_list_.remove(node);
	node->SetContainer(nullptr);
}

bool NodeList::Contains(Node* node) const
{
	return node->GetContainer() == this;
}

//int NodeList::Size()
//{
//	return static_cast<int>(node_list_.size());
//}

NodeList::~NodeList()
{
	for (auto* node : node_list_)
	{
		node->SetContainer(nullptr);
	}
}