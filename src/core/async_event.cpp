#include "core/async_event.h"
#include "core/manager.h"

using namespace asyncflow::core;

AsyncEventBase::AsyncEventBase(int id, Agent* agent)
	: id_(id)
	, arg_count_(0)	
{
	agent_id_ = agent == nullptr ? UINT64_MAX : agent->GetId();
}

NodeLinkedList* AsyncEventBase::GetWaitingNodes(const Manager& mgr) const
{
	auto agent =  mgr.GetAgentManager().GetAgentById(agent_id_);
	if (agent == nullptr)
		return nullptr;
	return agent->GetWaitNodes(id_, false);
}