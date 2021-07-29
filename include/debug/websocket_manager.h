#pragma once
#include "debug/debugger.h"

#ifdef FLOWCHART_DEBUG
#include <vector>
#include <unordered_map>
#include <set>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <unordered_set>



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
		class WebsocketManager
		{
			typedef std::vector<websocketpp::connection_hdl> HDL_CONTAINER;
		public:
			WebsocketManager(core::Manager* manager);
			~WebsocketManager();
			void Init(const std::string& ip, int port);
			void StopDebugChart(core::Chart* chart);
			void StartQuickDebug(core::Chart* chart);
			void ContinueDebugChart(core::Chart* chart);

		private:
			WebsocketAsioServer server_;
			core::Manager* manager_;
			std::unordered_map<core::Chart*, HDL_CONTAINER> chart_map_;
			std::unordered_map<std::string, HDL_CONTAINER> quick_debug_map_;			
			Debugger* debugger_;
			

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

		class WebsocketDebugConnection : public DebugConnection
		{
		public:
			WebsocketDebugConnection(WebsocketManager* websocketManager_, websocketpp::connection_hdl hdl)
				: websocketManager_(websocketManager_)
				, hdl_(hdl)
			{};

			void StartDebugChart(core::Chart*) override;
			void StopDebugChart(core::Chart*)  override;
			void QuickDebugChart(core::ChartData*) override;
			void ContinueDebugChart(core::Chart*) override;
			void Reply(const std::string& msg) override;


		private:
			WebsocketManager* websocketManager_;
			websocketpp::connection_hdl hdl_;
		};
	}
}
#endif