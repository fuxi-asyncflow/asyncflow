#ifdef FLOWCHART_DEBUG
#pragma once
#include "debug/debugger.h"
#include "rapidjson/prettywriter.h"
#include "debug_common.h"
#include <functional>
#include <unordered_map>

namespace asyncflow
{
    namespace debug
    {
		class JsonDebugger : public Debugger
		{
		public:
			JsonDebugger();
			~JsonDebugger() = default;

			void HandleMessage(const std::string& msg, core::Manager* manager_, DebugConnection* obj) override;
		
			std::string PrepareChartDebugData(Chart* chart) override;
			std::string StopMessage(const std::string& chart_name) override;
			std::string ChartInfo(Chart* chart) override;
			std::string HeartBeat() override;

		private:
			static void ChartDataToJson(core::ChartData* chart_data, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
			static std::string ErrorReply(int code, const std::string& error_msg, int id);
			static std::string SimpleReply(const std::string& method, const std::vector<std::string>& result, int id);
			static std::string SimpleReply(const std::string& method, const std::unordered_map<std::string, std::string>& result, int id);			
			static std::string ChartInitData(const std::string& method, core::ChartData* chart_data, int id);			
			static std::string GetChartList(core::Manager* manager, const std::string& obj_name, const std::string& chart_name, int id);
			static std::string GenChartInfo(Chart* chart);

			using MessageHandler = std::function<void(rapidjson::Document&, Manager*, DebugConnection*, int)>;		
			std::unordered_map<std::string, MessageHandler> handlers_;
			static void GetChartListHandler(rapidjson::Document& doc, Manager* manager, DebugConnection* connect, int manager_id);
			static void DebugChartHandler(rapidjson::Document& doc, Manager* manager, DebugConnection* connect, int manager_id);
			static void StopChartHandler(rapidjson::Document& doc, Manager* manager, DebugConnection* connect, int manager_id);
			static void QuickDebugHandler(rapidjson::Document& doc, Manager* manager, DebugConnection* connect, int manager_id);
			static void BreakPointHandler(rapidjson::Document& doc, Manager* manager, DebugConnection* connect, int manager_id);
			static void ContinueHandler(rapidjson::Document& doc, Manager* manager, DebugConnection* connect, int manager_id);
			static void GmHandler(rapidjson::Document& doc, Manager* manager, DebugConnection* connect, int manager_id);
			static void HotFixHandler(rapidjson::Document& doc, Manager* manager, DebugConnection* connect, int manager_id);
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