#ifdef FLOWCHART_DEBUG
#pragma once
#include "core/manager.h"
#include "core/node.h"
using namespace asyncflow;

namespace asyncflow
{
	namespace debug
	{
		class HandleObject
		{
		public:
			virtual void StartDebugChart(core::Chart*) = 0;
			virtual void StopDebugChart(core::Chart*) = 0;
			virtual void QuickDebugChart(core::ChartData*) = 0;
			virtual void ContinueDebugChart(core::Chart*) = 0;
			virtual void Reply(const std::string& msg) = 0;
		};

		class Debugger
		{
		public:
			static std::string GetChartList(core::Manager* manager, const std::string& obj_name, const std::string& chart_name, int id);
			static std::string FindChart(core::Manager* manager, int agent_id, const std::string& chart_name, core::Node* owner_node, core::Chart** pchart);
			static std::string DebugChart(core::Manager* manager, int agent_id, const std::string& chart_name
				, core::Node* owner_node, core::Chart** pchart, int id);
			static std::string StopChart(core::Manager* manager, int agent_id, const std::string& chart_name
				, core::Node* owner_node, core::Chart** pchart, int id);
			static std::string ContinueDebug(core::Manager* manager, int agent_id, const std::string& chart_name
				, core::Node* owner_node, core::Chart** pchart, int id);

			static std::string PrepareChartDebugData(core::Chart* chart);
			static void ChartDataToJson(core::ChartData* chart_data, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

			static std::string ErrorReply(int code, const std::string& error_msg, int id);
			static std::string NormalMessage(const std::string& method, const std::vector<std::string>& result, int id);
			static std::string SimpleReply(const std::string& method, const std::unordered_map<std::string, std::string>& result, int id);
			static std::string StopMessage(const std::string& chart_name);
			static std::string ChartInitData(const std::string& method, core::ChartData* chart_data, int id);
			static std::string ChartInfo(core::Chart* chart);

			static void HandleMessage(const std::string& msg, Manager* manager_, HandleObject* obj);
		};
	}
}
#endif