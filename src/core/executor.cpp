#include "core/executor.h"
#include "core/node.h"
#include "core/agent.h"
#include "core/manager.h"

using namespace asyncflow::core;
using namespace asyncflow::util;

DfsExecutor::DfsExecutor()
	: current_agent_(nullptr)
	, current_node_(nullptr)
{
}

bool DfsExecutor::RunFlow(Node* start_node)
{
	current_agent_ = start_node->GetAgent();
	if (current_agent_->GetStatus() != Agent::Status::Running) return true;

	auto* chart = start_node->GetChart();
	if (!chart->IsRunning())
		return true;

	bool has_running_node = false;
	
	start_node->SetStatus(Node::Idle);	//in Idle status before running
	
	ASYNCFLOW_DBG("\n\nstart run from node {0} : [{1} {2}][{3}]", (void*)start_node, (void*)(chart)
		, chart->Name(), start_node->GetId());

	node_list_.clear();
	node_list_.push_back(start_node);

	//Start with start node and depth-first traversal
	//RunFlag is set in node run function and is set to true only for nodes that are actually running
	while (!node_list_.empty() && chart->IsRunning())	// if chart is return or stopped as subChart, status will be set to Idle
	{
		auto* node = GetTopNode();		

		if (node->IsRunning() && !node->IsWaitAll())   //special handling WaitAll node
		{
			has_running_node = true;
			PopNode();
			continue;
		}

		if (loop_check_ && node->RunFlag() && !node->GetData()->IsEventNode())	//loop
		{
			current_agent_->WaitEvent(node, AsyncEventBase::TICK_EVENT);
			has_running_node = false;
		}
		else
		{
#ifdef ENABLE_PERF
			auto start_time = std::chrono::high_resolution_clock::now();
#endif
			current_node_ = node;
			node->Run();

#ifdef ENABLE_PERF
			auto end_time = std::chrono::high_resolution_clock::now();
			auto cost = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
			node->GetData()->AddRunCount();
			node->GetData()->AddTimeCost(cost);
#endif
			PopNode(); // pop node after node run end, so if current node is `stopflow` and stop self, it's children would not push into queue

			//If the node finishes executing(not in running stateus),the subsequent node is added to the list			
			if (node->IsRunning())
				has_running_node = true;
			else
			{
				node->SetStatus(Node::EndRun);
				AddSubsequenceNodes(node, node->GetResult());
			}
		}
	}

	//restore finished executing nodes(not in running status)
	for (auto node : history_nodes_)
	{
		node->SetRunFlag(false);
		if (node->GetStatus() == Node::EndRun)
		{
			node->Stop();
		}
	}
	history_nodes_.clear();

	//check running nodes in chart	
	if (!has_running_node && !chart->CheckRunning())
	{
		//a chart that has no return node returns true by default
		if (chart->GetStatus() == Chart::Status::Running)
		{
			chart->Return(true);
			chart->SetStatus(Chart::Status::Idle);
		}
#ifdef	FLOWCHART_DEBUG
		// common chart will stop debugging
		// subchart will not stop debugging while owner chart is running
		bool isSubChart = chart->GetOwnerNode() != nullptr;
		if ((!isSubChart && chart->IsDebug())
			|| (isSubChart && !chart->GetOwnerNode()->GetChart()->IsDebug()))
		{
			chart->GetAgent()->GetManager()->GetWebsocketManager().StopDebugChart(chart);
		}
#endif
	}
	ASYNCFLOW_DBG("end run from node {0}", (void*)start_node);
	current_node_ = nullptr;
	current_agent_ = nullptr;

	return true;
}

void DfsExecutor::AddSubsequenceNodes(Node* node, NodeResult result)
{
	if (result == rERROR)
		return;
	const auto& ids = node->GetData()->GetSubsequenceIds(result == rTRUE);
	auto chart = node->GetChart();

	//DFS push the right node into stack first
	auto count = ids.size();
	while (count > 0)
	{
		auto child = chart->GetNode(ids[--count]);
		child->SetPreNodeId(node->GetId());
		ASYNCFLOW_DBG("{0} -> {1}", node->GetId(), child->GetId());
		node_list_.push_back(child);
	}
}
