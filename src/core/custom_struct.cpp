#include "core/custom_struct.h"

using namespace asyncflow::core;

void NodeList::PushBack(Node* node)
{
	if (auto list = node->GetWaitingList())
	{
		list->Remove(node);
	}
	node_list_.push_back(node);
	node->SetWaitingList(this);
}

Node* NodeList::PopFront()
{
	auto node = node_list_.front();
	node_list_.pop_front();
	node->SetWaitingList(nullptr);
	return node;
}

void NodeList::Remove(Node* node)
{
	node_list_.remove(node);
	node->SetWaitingList(nullptr);
}

int NodeList::Size()
{
	return static_cast<int>(node_list_.size());
}

NodeList::~NodeList()
{
	for (auto* node : node_list_)
	{
		node->SetWaitingList(nullptr);
	}
}