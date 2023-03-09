#include "core/agent.h"
#include "core/async_event.h"
#include "core/node.h"
#include "core/manager.h"
#include "util/log.h"
#include <algorithm>
#include "core/custom_struct.h"

using namespace asyncflow::core;
using namespace asyncflow::util;

std::string Agent::DEBUG_NAME_METHOD = "GetAsyncFlowName";

Agent::Agent(Manager* manager)
	: id_(AGENT_INIT_ID)
	, tick_interval_(Manager::DEFAULT_AGENT_TICK)
	, status_(Running)
	, manager_(manager)
	, timer_(this)
{
	timer_.Start();
	for (auto i = 0; i <= manager_->GetEventManager().GetEventCount(); i++)
	{
		waiting_nodes_list.push_back(new NodeList());
	}
}

Agent::~Agent()
{
	ASYNCFLOW_DBG("~Agent {0}", (void*)this);
	timer_.Stop();

	// destroy all nodes that are waiting for events
	for (auto* v : waiting_nodes_list)
	{
		delete v;
	}
	waiting_nodes_list.clear();

	// destroy all charts
	for (auto& chart_list : chart_dict_)
	{
		std::vector<Chart*> tmp(chart_list.second.begin(), chart_list.second.end());
		for (auto* chart : tmp)
		{
			auto* owner_node = chart->GetOwnerNode();
			if (owner_node != nullptr)
				owner_node->SetAttacher(nullptr);
			else
				delete chart;
		}
	}
}

Chart* Agent::AttachChart(ChartData* chart_data)
{
	Chart* chart = manager_->CreateChart();	

	if (!chart->Init(chart_data))
		return nullptr;

	if (!AddChart(chart))
	{
		delete chart;
		return nullptr;
	}
#ifdef FLOWCHART_DEBUG
	if (chart_data->IsQuickDebug())
	{
		manager_->GetWebsocketManager().StartQuickDebug(chart);
		chart_data->SetQuickDebug(false);
	}
#endif
	return chart;
}

bool Agent::AddChart(Chart* chart, Node* node /* = nullptr*/)
{
	if (chart == nullptr) return false;
	auto& chart_name = chart->Name();
	auto it = chart_dict_.find(chart_name);
	if (it == chart_dict_.end())
	{
		chart_dict_.emplace(chart_name, std::vector<Chart*>{});
		it = chart_dict_.find(chart_name);
	}
	auto& chart_list = it->second;

	auto const it_chart = std::find_if(chart_list.begin(), chart_list.end(),
		[node](Chart* c)->bool { return c->GetOwnerNode() == node; });
	if (it_chart != chart_list.end())
	{
		ASYNCFLOW_WARN("agent has chart with same name and same owner node");
		return false;
	}
	chart->SetAgent(this);
	chart_list.push_back(chart);
	return true;
}

bool Agent::StartChart(const std::string& chart_name)
{
	auto* chart = FindChart(chart_name, nullptr);
	if (chart != nullptr && chart->GetStatus() != Chart::Status::Running)
	{
		this->StartChart(chart);
		return true;
	}
	return false;
}

bool Agent::StartChart(Chart* chart, bool sync, void* args, int argc)
{
	if (chart->GetStatus() == Chart::Running)
		return false;
	chart->ResetVariables();
	if(argc > 0)
		chart->SetArgs(args, argc);
	chart->SetStatus(Chart::Running);
	auto* start_node = chart->GetNode(0);
	// if has start function, it will not be skipped
	start_node->SetSkip(chart->GetData()->StartFuncName().empty());

	start_node->SetStatus(Node::Running);
	if(sync)
	{
		// if out of stack, then try async mode
		if (manager_->GetExecutor().RunFlow(start_node))
			return true;
	}
	
	WaitEvent(start_node, AsyncEventBase::START_EVENT);
	manager_->Event(AsyncEventBase::START_EVENT, this, nullptr, 0);
	return true;
}

// this function should only be used in ~Chart()
bool Agent::EraseChart(Chart* chart)
{
	auto& chart_name = chart->Name();
	auto it = chart_dict_.find(chart_name);
	if (it == chart_dict_.end())
	{
		return false;
	}

	auto& chart_list = it->second;
	chart->SetAgent(nullptr);
	chart_list.erase(std::remove(chart_list.begin(), chart_list.end(), chart));
	return true;
}

bool Agent::RemoveChart(const std::string& chart_name)
{
	auto* chart = FindChart(chart_name, nullptr);
	if (chart != nullptr)
	{
		delete chart;
		return true;
	}
	ASYNCFLOW_WARN("{0} has not attached to the object", chart_name);
	return false;
}


NodeList* Agent::GetWaitNodes(int event_id)
{
	if (event_id < 0 || event_id >= manager_->GetEventManager().GetEventCount())
	{
		ASYNCFLOW_WARN("event id is out of range : {0}", event_id);
		return nullptr;
	}
	return waiting_nodes_list[event_id];
}

void Agent::HandleEvent(const AsyncEventBase& event, NodeList* waiting_nodes)
{
	if (status_ == Destroying)	return;

	// As the node runs, new nodes may be added to the list, so a copy is created.
	waiting_nodes_list[event.Id()] = new NodeList();
	ASYNCFLOW_DBG("handle event {0} for agent {1} [{2}-{4}], total {3} nodes",
		(void*)&event, (void*)this, event.Id(), waiting_nodes->Size(), manager_->GetEventManager().GetEventName(event.Id()));

	while (!waiting_nodes->IsEmpty())
	{
		// The agent of the node may be different from the agent which the current event belongs to, eg $obj.OnSee.
		auto* node = waiting_nodes->Pop();
		auto* agent = node->GetAgent();
		node->SetStatus(Node::Idle);
#ifdef FLOWCHART_DEBUG
		node->SendEventStatus(&event);
#endif
		agent->RunFlow(node);
	}	
	// assert(waiting_nodes->empty());	
	// Assert may be failed: As subchart called, the start node of the subchart is also waiting for the start event and is placed in the same list, which cannot be cleared
}

// RunFlow function is only used in two places: event occures, and timer-triggered node runs.
void Agent::RunFlow(Node* start_node)
{
	manager_->GetExecutor().RunFlow(start_node);
}

// Start to run all charts that are not in running status;
int Agent::Start()
{
	int count = 0;
	for (const auto& charts : chart_dict_)
	{
		for (auto* chart : charts.second)
		{
			if (chart->GetOwnerNode() == nullptr && chart->GetStatus() != Chart::Status::Running)
			{
				if (StartChart(chart))
					count++;
			}
		}
	}
	return count;
}

void Agent::SetTickInterval(int tick)
{
	tick_interval_ = tick;
	if (status_ == Running)
	{
		timer_.Stop();
		timer_.Start();
	}
}

// Add nodes to the waiting list.
void Agent::WaitEvent(Node* node, int event_id)
{
	auto* waiting_nodes = GetWaitNodes(event_id);
	if (waiting_nodes == nullptr)
	{
		assert(event_id);
		ASYNCFLOW_WARN("event id is out of range : {0}", event_id);
		return;
	}
	ASYNCFLOW_DBG("node {0} : {1}[{2}] wait event {3} [{4}-{5}]"
		, (void*)node, node->GetChart()->Name(), node->GetId(), (void*)this, event_id, manager_->GetEventManager().GetEventName(event_id));
	node->SetContainer(nullptr);
	waiting_nodes->Push(node);
	// The node waiting for an event must be Running status.
	node->SetStatus(Node::Running);			
}

std::vector<std::string> Agent::GetRunningChartNames()
{
	std::vector<std::string> v;
	for (auto& charts : chart_dict_)
	{
		for (auto& chart : charts.second)
		{
			if (chart->GetStatus() == Chart::Running && chart->GetOwnerNode() == nullptr)
			{
				v.push_back(chart->Name());
			}
		}
	}
	return v;
}


Chart* Agent::FindChart(const std::string& chart_name, Node* owner_node)
{
	auto it = chart_dict_.find(chart_name);
	if (it == chart_dict_.end())
		return nullptr;
	for (auto* chart : it->second)
	{
		if (chart->GetOwnerNode() == owner_node)
		{
			return chart;
		}
	}
	return nullptr;
}

void Agent::Stop()
{
	for (const auto& chart_list : chart_dict_)
	{
		for (auto* chart : chart_list.second)
		{
			chart->Stop();
		}
	}
}

bool Agent::StopChart(const std::string& chart_name)
{
	auto* chart = FindChart(chart_name, nullptr);
	if (chart != nullptr)
	{
		chart->Stop();
		return true;
	}
	return false;
}