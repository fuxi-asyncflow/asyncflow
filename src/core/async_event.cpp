#include "core/async_event.h"
#include "core/manager.h"

using namespace asyncflow::core;

AsyncEventBase::AsyncEventBase(int id, Agent* agent)
	: id_(id)
	, arg_count_(0)	
{
	agent_id_ = agent == nullptr ? UINT64_MAX : agent->GetId();
}

Agent* AsyncEventBase::GetAgent(const Manager& mgr) const
{
	return mgr.GetAgentManager().GetAgentById(agent_id_);	
}