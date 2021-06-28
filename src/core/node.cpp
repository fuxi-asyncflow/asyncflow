#include "core/node.h"
#include "core/agent.h"
#include "core/node_data.h"
#include "core/node_func.h"
#include "core/manager.h"
#include <algorithm>
#include "util/log.h"
using namespace asyncflow::core;

Node::Node(Chart* chart, NodeData* data)
	: attacher_(nullptr)
	, chart_(chart)
	, data_(data)
	, status_(Idle)
	, run_flag_(false)
	, result_(true)
	, waiting_list_(nullptr)
	, skip_(false)
	, pre_node_id_(-1)
	, is_wait_all(false)
{
	id_ = data_->GetId();
}

Node::~Node()
{
	Stop();
	if (attacher_ != nullptr)
	{
		SetAttacher(nullptr);
	}
}

Agent*	Node::GetAgent() const { return chart_->GetAgent(); }

bool Node::Run()
{
	ASYNCFLOW_DBG("--------------------- RUN NODE {0} : [{1}], skip:{2}", id_, chart_->Name(), skip_);
	//if the node does not execute, its runflag will not change
	if(!skip_)
	{
		this->SetRunFlag(true);
		auto func = data_->GetNodeFunc();
		if(func == nullptr)
		{
			ASYNCFLOW_ERR("node func is null");
			return false;
		}
		result_ = func->call(GetAgent());
		ASYNCFLOW_DBG("--------------------- RUN NODE RESULT {0}", result_);
	}
	else
	{		
		skip_ = false;
	}		
	return result_;
}

//restore nodes in initial state
void Node::Stop()
{	
	status_ = Idle;
	run_flag_ = false;
	skip_ = false;

	if(attacher_ != nullptr)
	{		
		attacher_->Stop();
	}
	
	if (waiting_list_ != nullptr)
	{
		waiting_list_->Remove(this);
	}
}

void Node::SetStatus(Status status)
{	
	if(status_ != status)
	{
#ifdef FLOWCHART_DEBUG
		if(chart_->IsDebug())
		{
			chart_->SendNodeStatus(data_->GetId(), data_->GetUid(), status_, status, result_);
		}
#endif
		status_ = status;
	}	
}

#ifdef FLOWCHART_DEBUG
void Node::SendEventStatus(const AsyncEventBase* event)
{
	if (chart_->IsDebug() && data_->IsEventNode())
	{
		chart_->SendEventStatus(data_->GetUid(), event);
	}
}
#endif