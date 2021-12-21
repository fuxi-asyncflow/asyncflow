#ifdef FLOWCHART_DEBUG
#pragma once
#include "debug/debugger.h"
#include "rapidjson/prettywriter.h"
#include "debug_common.h"

namespace asyncflow
{
    namespace debug
    {
		class JsonDebugger : public Debugger
		{
		public:
			std::string PrepareChartDebugData(core::Chart* chart) override;
			void ChartDataToJson(core::ChartData* chart_data, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
			std::string ErrorReply(int code, const std::string& error_msg, int id) override;
			std::string SimpleReply(const std::string& method, const std::vector<std::string>& result, int id) override;
			std::string SimpleReply(const std::string& method, const std::unordered_map<std::string, std::string>& result, int id) override;
			std::string StopMessage(const std::string& chart_name) override;
			std::string ChartInitData(const std::string& method, core::ChartData* chart_data, int id) override;
			std::string ChartInfo(core::Chart* chart) override;
			std::string GetChartList(core::Manager* manager, const std::string& obj_name, const std::string& chart_name, int id) override;
			std::string HeartBeat() override;

			void HandleMessage(const std::string& msg, core::Manager* manager_, DebugConnection* obj) override;
		};

		typedef rapidjson::PrettyWriter<rapidjson::StringBuffer> Writer;
		void Serialize_ChartInfo_Json(Writer& writer, const ChartInfo& ci);
		void Deserialize_ChartInfo_Json(const rapidjson::Value& obj, ChartInfo& ci);
		void Serialize_NodeStatusData_Json(Writer& writer, const NodeStatusData& nsd);
		void Deserialize_NodeStatusData_Json(const rapidjson::Value& obj, NodeStatusData& nsd);
		void Serialize_VariableStatusData_Json(Writer& writer, const VariableStatusData& vsd);
		void Deserialize_VariableStatusData_Json(const rapidjson::Value& obj, VariableStatusData& vsd);
		void Serialize_EventStatusData_Json(Writer& writer, const EventStatusData& esd);
		void Deserialize_EventStatusData_Json(const rapidjson::Value& obj, EventStatusData& esd);
    }
}

#endif