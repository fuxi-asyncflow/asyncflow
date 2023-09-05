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
			DebugChartInfo(uint64_t id, core::Node* node, core::Chart* chart, std::string name)
			    : agent_id(id), owner_node(node), chart(chart), chart_name(name)
			{
			    
			}
			uint64_t agent_id;
			core::Node* owner_node;
			core::Chart* chart;
			std::string chart_name;
		};

	    class Debugger
	    {
	    public:
			Debugger() = default;
			virtual ~Debugger() = default;
			virtual void HandleMessage(const std::string& msg, core::Manager* manager_, DebugConnection* obj) = 0;
	    
			virtual std::string PrepareChartDebugData(core::Chart* chart) = 0;
			virtual std::string StopMessage(const std::string& chart_name) = 0;
			virtual std::string ChartInfo(core::Chart* chart) = 0;
			virtual std::string HeartBeat() = 0;
		public:
			
			static std::string FindChart(core::Manager* manager, DebugChartInfo& chart_info);
	    };
		
	}
}
#endif