#pragma once
#ifdef FLOWCHART_DEBUG
#include <vector>
#include <unordered_map>
#include <set>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <unordered_set>
#include "util/json.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"


typedef websocketpp::server<websocketpp::config::asio> WebsocketAsioServer;

namespace asyncflow
{
	namespace core
	{
		class Manager;
		class Chart;
		class Node;
		class Agent;
		class ChartData;
	}

	namespace debug
	{
		class WebsocketHandleObject;
		class WebsocketManager
		{
			typedef std::vector<websocketpp::connection_hdl> HDL_CONTAINER;
		public:
			WebsocketManager(core::Manager* manager);
			~WebsocketManager();
			void Init(std::string ip = IP, int port = START_PORT);
			void StopDebugChart(core::Chart* chart);
			void StartQuickDebug(core::Chart* chart);
			void ContinueDebugChart(core::Chart* chart);

		private:
			WebsocketAsioServer server_;
			core::Manager* manager_;
			std::unordered_map<core::Chart*, HDL_CONTAINER> chart_map_;
			std::unordered_map<std::string, HDL_CONTAINER> quick_debug_map_;
			WebsocketHandleObject* handle_obj_;

		public:
			void StartDebugChart(core::Chart* chart, websocketpp::connection_hdl hdl);			
			void StopDebugChart(core::Chart* chart, websocketpp::connection_hdl hdl);
			void QuickDebugChart(core::ChartData* chart, websocketpp::connection_hdl hdl);
			void SendReply(websocketpp::connection_hdl hdl, const std::string& msg);

		private:
			void OnMessage(websocketpp::connection_hdl hdl, WebsocketAsioServer::message_ptr msg);
			bool IsPortAvailable(const char* ip, int port);

			bool ContainsHdl(const HDL_CONTAINER& container, websocketpp::connection_hdl& hdl);
			void RemoveHdl(HDL_CONTAINER& container, websocketpp::connection_hdl& hdl);
			void SendStopData(core::Chart* chart, HDL_CONTAINER &hdls);

		public:
			void Step();
			static int START_PORT;
			static std::string IP;

			//use for test
			const std::unordered_map<core::Chart*, HDL_CONTAINER>& GetChartMap() { return chart_map_; }
						
		};
	}
}
#endif