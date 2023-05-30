#include "core/chart.h"
#include "core/chart_data.h"
#include "core/agent.h"
#include "util/log.h"
#include "core/async_event.h"
#include "core/manager.h"

using namespace asyncflow::core;

Chart::Chart(Manager* mgr)
	: owner_node_(nullptr)
	, data_(nullptr)
	, agent_(nullptr)
	, status_(Idle)			
	, debug_(false)
	, debug_data_count_(0)
    , uid_(mgr->uuid4_str())
{
	
}

Chart::~Chart()
{
	// printf("~chart %p ownernode: %p agnet %p\n", this, owner_node_, agent_);
	ASYNCFLOW_DBG("~Chart {0} {1}", (void*)this, Name());
#ifdef	FLOWCHART_DEBUG
	if (IsDebug())
	{
		if (agent_ != nullptr)
		{
			agent_->GetManager()->GetWebsocketManager().StopDebugChart(this);
		}
	}
	ClearDebugData();
#endif
	for (auto* node : node_list_)
	{
		delete node;
	}
	node_list_.clear();
	
	//remove from agent
	if (agent_ != nullptr)
	{
		if (!agent_->EraseChart(this))
		{
			ASYNCFLOW_ERR("remove chart {0} from agent failed in ~Chart", (void*)this);
		}
	}

	//prevent the owner chart from repeating destructing the subchart
	if (owner_node_ != nullptr)
	{
		owner_node_->SetAttacherNull();
	}
}



bool Chart::Init(ChartData* chart_data, Node* owner_node /* = nullptr*/)
{
	// printf("+chart %p owner_node %p\n", this, owner_node);
	data_ = chart_data;	
	assert(owner_node_ == nullptr || owner_node_ == owner_node);
	owner_node_ = owner_node;
	const int node_count = data_->GetNodeCount();
	node_list_.clear();
	for (int i = 0; i < node_count; i++)
	{
		node_list_.emplace_back(new Node(this, data_->GetNodeData(i)));
	}
	return true;
}

//Stop function contains a virtual function ClearVariables, so it can not be used in chart destructor.
void Chart::Stop()
{
	for (auto* node : node_list_)
	{
		node->Stop();
	}
	status_ = Idle;
	ClearVariables();
}

void Chart::Return(bool result)
{	
	auto* owner_node = this->GetOwnerNode();
	if (owner_node != nullptr)
	{	
		owner_node->Stop();		
		owner_node->SetResult(result);
		owner_node->SetSkip(true);
		owner_node->SetRunFlag(false);
		
		auto* agent = owner_node->GetAgent();
		auto* manager = agent->GetManager();
		if(manager->IsImmediateSub() && owner_node->GetAgent()->RunFlow(owner_node)) // not stack over flow
		{
			
		}
		else
		{
			agent->WaitEvent(owner_node, AsyncEventBase::START_EVENT);
			agent->GetManager()->Event(AsyncEventBase::START_EVENT, agent
				, nullptr, 0, true);
		}
		
	}
	ASYNCFLOW_DBG("chart {0} {1} return", (void*)this, Name());
	Stop();
}

bool Chart::CheckRunning()
{
	return std::any_of(node_list_.begin(), node_list_.end(), 
		[](Node* node) { return node->IsRunning(); });
}

#ifdef 	FLOWCHART_DEBUG
void Chart::SendNodeStatus(int id, const std::string& uid, Node::Status old_status, Node::Status new_status, bool result)
{
	if (old_status != new_status)
	{
		auto* data = new debug::NodeStatusData(id, uid, old_status, new_status, result);
		data->id = debug_data_count_++;
		debug_data_list_.push_back(data);
	}

}

void Chart::ClearDebugData()
{
	for (auto* debug_data : debug_data_list_)
	{
		delete debug_data;
	}
	debug_data_list_.clear();
}
#endif