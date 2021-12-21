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

std::string Debugger::DebugChart(Manager* manager, DebugChartInfo& chart_info, int msg_id)
{
	const auto reply = FindChart(manager, chart_info);
	//ensure agent is not destroyed
	if (chart_info.chart == nullptr)
	{
		return ErrorReply(-1, reply, msg_id);
	}
	else
	{
		ASYNCFLOW_DBG("[debug] DebugChart {0} {1} {2}", chart_info.agent_id, chart_info.chart_name, (void*)chart_info.owner_node);
		return ChartInitData("debug_chart", chart_info.chart->GetData(), msg_id);
	}
}

std::string Debugger::ContinueDebug(Manager* manager, DebugChartInfo& chart_info, int msg_id)
{
	ASYNCFLOW_DBG("[debug] ContinueChart {0} {1} {2}", chart_info.agent_id, chart_info.chart_name, (void*)chart_info.owner_node);
	const auto reply = FindChart(manager, chart_info);
	if (chart_info.chart == nullptr)
	{
		return ErrorReply(-1, reply, msg_id);
	}
	else
	{
		std::unordered_map<std::string, std::string> result;
		result["chart_name"] = chart_info.chart_name;
		return SimpleReply("continue", result, msg_id);
	}
}

std::string Debugger::StopChart(Manager* manager, DebugChartInfo& chart_info, int id)
{
	ASYNCFLOW_DBG("[debug] StopDebugChart {0} {1} {2}", chart_info.agent_id, chart_info.chart_name, (void*)chart_info.owner_node);
	auto reply = FindChart(manager, chart_info);
	if (chart_info.chart == nullptr)
	{
		return ErrorReply(-1, reply, id);
	}
	else
	{
		std::unordered_map<std::string, std::string> result;
		result["chart_name"] = chart_info.chart_name;
		return SimpleReply("stop_chart", result, id);
	}
}


#endif