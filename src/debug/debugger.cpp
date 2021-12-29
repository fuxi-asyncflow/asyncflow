#ifdef FLOWCHART_DEBUG
#include "debug/debugger.h"
#include "util/log.h"
#include "core/manager.h"

using namespace asyncflow;
using namespace asyncflow::debug;

std::string Debugger::FindChart(core::Manager* manager, DebugChartInfo& chart_info)
{
	auto* agent = manager->GetAgentManager().GetAgentById(chart_info.agent_id);
	//ensure agent is not destroyed
	if (agent == nullptr)
	{
		ASYNCFLOW_WARN("[debug] agent {0} does not exist", chart_info.agent_id);
		std::string error_msg = "agent_id " + std::to_string(chart_info.agent_id) + " not exist";
		return error_msg;
	}
	auto chart = agent->FindChart(chart_info.chart_name, chart_info.owner_node);
	if (chart == nullptr)
	{
		ASYNCFLOW_WARN("[debug] Chart {0} with agent id {1} ownernode {2} does not exist", chart_info.chart_name, chart_info.agent_id, (void*)chart_info.owner_node);
		std::string error_msg = "chart_name " + chart_info.chart_name + " does not exist";
		return error_msg;
	}
	chart_info.chart = chart;
	return "";

}
#endif