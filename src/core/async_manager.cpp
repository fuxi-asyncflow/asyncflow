#include "core/async_manager.h"
#include "core/node.h"
#include "core/chart.h"
#include "core/agent.h"
#include "core/node_func.h"

using namespace asyncflow::core;

void AsyncManager::Step()
{
	while(activate_nodes_.Size() > 0)
	{
		auto* node = activate_nodes_.Pop();
		auto const flag = node->GetSkip();
		auto const result = node->GetResult();
		node->Stop();	//TODO why stop node here?
		node->SetSkip(flag);
		node->SetResult(result);
		RemoveNode(node);
		auto* agent = node->GetAgent();
		agent->RunFlow(node);
	}
}


void AsyncManager::AddNode(Node* node)
{
	waiting_nodes_.Push(node);
	nodes_set_.insert(node);
}

bool AsyncManager::IsNodeWaiting(Node* node)
{
	return nodes_set_.find(node) != nodes_set_.end();
}

void AsyncManager::RemoveNode(Node* node)
{
	waiting_nodes_.Remove(node);
	nodes_set_.erase(node);
}

void AsyncManager::ActivateNode(Node* node, bool skip_flag /*= true*/)
{
	if (activate_nodes_.Contains(node))
		return;
	activate_nodes_.Push(node);
	node->SetSkip(skip_flag);
}

#ifdef FLOWCHART_DEBUG
void AsyncManager::ContinueBreakpoint(Chart* chart)
{
	const auto& node_list = waiting_nodes_.GetList();
	for (auto* n : node_list)
	{
		if (n->GetChart() == chart && n->GetData()->IsBreakPoint())
		{
			auto* func = dynamic_cast<BreakpointFunc*>(n->GetData()->GetNodeFunc());
			if (func == nullptr)
				ASYNCFLOW_WARN("Continue Breakpoint error, node {} in chart {} is not breakpoint.", n->GetData()->GetUid(), chart->Name());
			else
			{
				func->SetExecute(true);
			}
			ActivateNode(n, false);
		}
	}
}

// continue to run the nodes in waiting_nodes_ those are deleted breakpoint
void AsyncManager::ContinueBreakpoint(NodeData* node_data)
{
	const auto& node_list = waiting_nodes_.GetList();
	for (auto* n : node_list)
	{
		if (n->GetData() == node_data)
		{
			ActivateNode(n, false);
		}
	}
}
#endif