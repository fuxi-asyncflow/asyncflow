#include "core/async_manager.h"
#include "core/node.h"
#include "core/chart.h"
#include "core/agent.h"
#include "core/node_func.h"

using namespace asyncflow::core;

void AsyncManager::Step()
{
	for (auto* node : activate_nodes)
	{
		auto const flag = node->GetSkip();
		node->Stop();
		node->SetSkip(flag);
		RemoveNode(node);
		auto* agent = node->GetAgent();
		agent->RunFlow(node);
	}
	activate_nodes.clear();
}


void AsyncManager::AddNode(Node* node)
{
	waiting_nodes_.PushBack(node);
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
	if (std::find(activate_nodes.begin(), activate_nodes.end(), node) != activate_nodes.end())
		return;
	activate_nodes.push_back(node);
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