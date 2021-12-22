#pragma once

#ifdef FLOWCHART_DEBUG
#include <string>
#include <vector>
#include <unordered_map>

namespace asyncflow
{
	namespace core
	{
		class Node;
		class Chart;
		class ChartData;
		class Manager;
	}
	namespace debug
	{
		class DebugConnection
		{
		public:
			virtual void StartDebugChart(core::Chart*) = 0;
			virtual void StopDebugChart(core::Chart*) = 0;
			virtual void QuickDebugChart(core::ChartData*) = 0;
			virtual void ContinueDebugChart(core::Chart*) = 0;
			virtual void Reply(const std::string& msg) = 0;
		};

		struct DebugChartInfo
		{
			DebugChartInfo(int id, core::Node* node, core::Chart* chart, std::string name)
			    : agent_id(id), owner_node(node), chart(chart), chart_name(name)
			{
			    
			}
			int agent_id;
			core::Node* owner_node;
			core::Chart* chart;
			std::string chart_name;			
		};

	    class Debugger
	    {
	    public:
			Debugger() = default;
			virtual void HandleMessage(const std::string& msg, core::Manager* manager_, DebugConnection* obj) = 0;

			virtual std::string PrepareChartDebugData(core::Chart* chart) = 0;
			virtual std::string ErrorReply(int code, const std::string& error_msg, int msg_id) = 0;
			virtual std::string SimpleReply(const std::string& method, const std::vector<std::string>& result, int msg_id) = 0;
			virtual std::string SimpleReply(const std::string& method, const std::unordered_map<std::string, std::string>& result, int msg_id) = 0;
			virtual std::string StopMessage(const std::string& chart_name) = 0;
			virtual std::string ChartInitData(const std::string& method, core::ChartData* chart_data, int msg_id) = 0;
			virtual std::string ChartInfo(core::Chart* chart) = 0;
			virtual std::string GetChartList(core::Manager* manager, const std::string& obj_name, const std::string& chart_name, int msg_id) = 0;
			virtual std::string HeartBeat() = 0;
		public:
			
			std::string FindChart(core::Manager* manager, DebugChartInfo& chart_info);
			std::string DebugChart(core::Manager* manager, DebugChartInfo& chart_info, int msg_id);
			std::string StopChart(core::Manager* manager, DebugChartInfo& chart_info, int msg_id);
			std::string ContinueDebug(core::Manager* manager, DebugChartInfo& chart_info, int msg_id);
	    };

		
	}
}
#endif