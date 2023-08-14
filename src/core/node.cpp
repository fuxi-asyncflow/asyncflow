#include "core/node.h"
#include "core/agent.h"
#include "core/node_data.h"
#include "core/node_func.h"
#include "core/manager.h"
#include <algorithm>
#include "util/log.h"
using namespace asyncflow::core;

Node::Node(Chart* chart, NodeData* data)
	: pre_node_id_(-1)
	, true_count_(0)
	, false_count_(0)
	, chart_(chart)
	, data_(data)
	, result_(rTRUE)
	, status_(Idle)
	, run_flag_(false)
	, skip_(false)
	, is_wait_all(false)
	, attacher_(nullptr)
	, container_(nullptr)
{
	// printf("+node %p chart: %p\n", this, chart_);
	id_ = data_->GetId();
}

Node::~Node()
{
	// printf("~node %p attacher: %p\n", this, attacher_);
	Stop();
	if (attacher_ != nullptr)
	{
		SetAttacher(nullptr);
	}
}

Agent*	Node::GetAgent() const { return chart_->GetAgent(); }

NodeResult Node::Run()
{
	ASYNCFLOW_DBG("--------------------- RUN NODE {0} : [{1}], skip:{2}", id_, chart_->Name(), skip_);
	//if the node does not execute, its runflag will not change
	if(!skip_)
	{
		this->SetRunFlag(true);
		SetResult(rFALSE);
		auto func = data_->GetNodeFunc();
		if(func == nullptr)
		{
			ASYNCFLOW_ERR("node func is null");
			return rFALSE;
		}
		auto r = func->call(GetAgent());
		if (result_ != rSTOP)
			SetResult(r);
		ASYNCFLOW_DBG("--------------------- RUN NODE RESULT {0}", result_);
	}
	else
	{		
		skip_ = false;
		//result_ = rTRUE;
	}		
	return result_;
}

//restore nodes in initial state
void Node::Stop()
{	
	status_ = Idle;
	result_ = rSTOP;
	run_flag_ = false;
	skip_ = false;

	if(attacher_ != nullptr)
	{		
		attacher_->Stop();
	}
	
	if (container_ != nullptr)
	{
		container_->Remove(this);
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