#include "core/node_attacher.h"
#include "core/manager.h"
#include <util/log.h>
using namespace asyncflow::core;

NodeTimer::NodeTimer(Node* node, int milliseconds)
	: INodeAttacher(node, TIMER)
	, time_interval_(milliseconds)	
{
	Register(node, milliseconds);	
}

NodeTimer::~NodeTimer()
{
	assert(node_->GetAttacher() == this);
	cancel();
}

void NodeTimer::Register(Node* node, uint64_t millisecond)
{
	cancel();
	time_interval_ = millisecond;
    auto* manager = node->GetChart()->GetAgent()->GetManager();
	manager->GetTimerManager().AddTimer(this, millisecond);
}

void NodeTimer::execute()
{
	auto* manager = node_->GetChart()->GetAgent()->GetManager();	
	node_->SetResult(rTRUE);
	ASYNCFLOW_DBG("node timer execute at {0}", manager->GetTimerManager().Now());
	manager->RunFlow(node_);	
}

void NodeTimer::Stop()
{
	cancel();
}

Subchart::Subchart(Node* node)
	: INodeAttacher(node, SUBCHART)
	, chart_(nullptr)
{

}

Subchart::~Subchart()
{
	delete chart_;
	chart_ = nullptr;
}

void Subchart::Stop()
{
	chart_->Stop();
}

///////////////////////////////////////////////////////////////////////////////

NodeWaitAll::NodeWaitAll(Node* node)
	: INodeAttacher(node, WAITALL)
{
	
}

NodeWaitAll::~NodeWaitAll()
{

}

void NodeWaitAll::Init(const std::vector<int>& args)
{
	node_ids_.clear();
	for(auto id: args)
	{
		node_ids_.emplace(id, false);
	}
}

bool NodeWaitAll::Run(int id)
{
	auto it = node_ids_.find(id);
	if(it != node_ids_.end())
	{
		it->second = true;
		//check if all node has runned
		for(auto iter = node_ids_.begin(); iter != node_ids_.end(); ++iter)
		{
			if (!iter->second)
				return false;			
		}
		//reset
		for(auto iter = node_ids_.begin(); iter != node_ids_.end(); ++iter)
		{
			iter->second = true;
		}
		return true;
	}
	else
	{
		ASYNCFLOW_WARN("waitall invalid id {0}", id);
	}
	return false;
}

void NodeWaitAll::Stop()
{
	
}