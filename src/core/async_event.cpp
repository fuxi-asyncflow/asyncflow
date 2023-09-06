#include "core/async_event.h"
#include "core/manager.h"

using namespace asyncflow::core;

AsyncEventBase::AsyncEventBase(int id, Agent* agent)
	: id_(id)
	, arg_count_(0)	
{
	//agent_id_ = agent == nullptr ? UINT64_MAX : agent->GetId();
	if (agent == nullptr)
		node_list_ = nullptr;
	else
	{
		auto* node_list = agent->GetWaitNodes(id, false);
		if (node_list == nullptr || node_list->IsEmpty())
			node_list_ = nullptr;
		else
		{
			node_list_ = new NodeLinkedList;
			node_list_->swap(*node_list);
		}
	}
}

AsyncEventBase::~AsyncEventBase()
{
	if (node_list_)
	{
		delete node_list_;
		node_list_ = nullptr;
	}
}


NodeLinkedList* AsyncEventBase::GetWaitingNodes(const Manager& mgr) const
{
	return node_list_;
	//auto agent =  mgr.GetAgentManager().GetAgentById(agent_id_);
	//if (agent == nullptr)
	//	return nullptr;
	//return agent->GetWaitNodes(id_, false);
}