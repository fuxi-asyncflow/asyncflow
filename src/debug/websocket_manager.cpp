#ifdef FLOWCHART_DEBUG
#include "debug/websocket_manager.h"
#include "core/manager.h"
#include "util/json.h"
#include "debug/json_debugger.h"


using namespace asyncflow::debug;
using namespace asyncflow::core;

int WebsocketManager::START_PORT = 9000;
std::string WebsocketManager::IP = "127.0.0.1";

WebsocketManager::WebsocketManager(Manager* manager)
	: manager_(manager)   
    , debugger_(new JsonDebugger())
{
	
	ASYNCFLOW_LOG("[deubg] Launch debug server !");
}

WebsocketManager::~WebsocketManager()
{
	delete debugger_;
}

void WebsocketManager::Init(const std::string& ip, int port)
{
#ifndef BUILD_WASM
	try
	{
		server_.init_asio();
		server_.clear_access_channels(websocketpp::log::alevel::all);
		server_.set_message_handler([this](websocketpp::connection_hdl hdl, WebsocketAsioServer::message_ptr msg)
			{
				OnMessage(hdl, msg);
			});

		//check available port start from port
		int max_delta = 100;
		for (int i = 0; i < max_delta; i++)
		{
			if (IsPortAvailable(ip.c_str(), port + i))
			{
				server_.listen(asio::ip::address::from_string(ip), port + i);
				server_.start_accept();
				ASYNCFLOW_LOG("[deubg] Init debug server success for ip {0} port {1}", ip, port + i);
				break;
			}
		}
	}
	catch(const std::exception& e)
	{
		ASYNCFLOW_LOG("[deubg] Init websocket manager error {}", e.what());
	}
	
#endif
}

void WebsocketManager::Step()
{
#ifndef BUILD_WASM
	ASYNCFLOW_DBG("[deubg] debugging chart count {0}", chart_map_.size());
	//send data
	std::vector<Chart *> charts_not_debugging;
	for (auto& chart_kv : chart_map_)
	{
		std::string str;
		auto* chart = chart_kv.first;
		auto& conns = chart_kv.second;
		if (chart->GetDebugData().empty())
			str = debugger_->HeartBeat();
		else
			str = debugger_->PrepareChartDebugData(chart);
		ASYNCFLOW_DBG("[deubg] chart json {0}", str);
		auto iter = conns.begin();
		while (iter != conns.end())
		{
			try
			{
				server_.send(*iter, str, websocketpp::frame::opcode::TEXT);
				++iter;
			}
			catch (const std::exception& e)
			{
				iter = conns.erase(iter);
				if (conns.empty())
					charts_not_debugging.push_back(chart);
				ASYNCFLOW_WARN("[deubg] websocket send error for chart {}, the reason is {}", chart->Name(), e.what());
			}
		}
	}

	// clear chart which not debugging any more
	for (auto* chart : charts_not_debugging)
	{
		chart->StopDebug();
		chart_map_.erase(chart_map_.find(chart));
	}

	try
	{
		server_.poll();
		if (server_.stopped())
			server_.reset();
	}
	catch (const std::exception& e)
	{
		ASYNCFLOW_WARN("[deubg] websocket step error {0}", e.what());
	}
	
#endif

}

bool WebsocketManager::ContainsHdl(const HDL_CONTAINER& container, websocketpp::connection_hdl& hdl)
{
	for (auto h : container)
	{
		if (h.lock() == hdl.lock())
		{
			return true;
		}
	}
	return false;
}

void WebsocketManager::RemoveHdl(HDL_CONTAINER& container, websocketpp::connection_hdl& hdl)
{
	auto it = std::find_if(container.begin(), container.end(),
		[&hdl](websocketpp::connection_hdl& a) -> bool {
		return a.lock() == hdl.lock();
	});

	if (it != container.end())
	{
		container.erase(it);
	}
}

void WebsocketManager::StartDebugChart(Chart* chart, websocketpp::connection_hdl hdl)
{
	if (chart == nullptr) return;
	auto it = chart_map_.find(chart);
	if (it == chart_map_.end())
	{
		it = chart_map_.insert({ chart, HDL_CONTAINER() }).first;
	}
	auto& hdls = it->second;

	if (!ContainsHdl(hdls, hdl))
	{
		hdls.insert(hdls.end(), hdl);
		chart->StartDebug();

		for (auto* node : chart->GetNodeList())
		{
			if (node->GetStatus() == Node::Running)
			{
				chart->SendNodeStatus(node->GetData()->GetId(), node->GetData()->GetUid(),
					Node::Idle, Node::Running, true);
			}

		}
	}
	return;
}

void WebsocketManager::QuickDebugChart(ChartData* chart_data, websocketpp::connection_hdl hdl)
{
	auto chart_name = chart_data->Name();
	auto it = quick_debug_map_.find(chart_name);
	if (it == quick_debug_map_.end())
	{
		it = quick_debug_map_.insert({ chart_name, HDL_CONTAINER() }).first;
	}
	auto& hdls = it->second;

	if (!ContainsHdl(hdls, hdl))
	{
		hdls.insert(hdls.end(), hdl);
		chart_data->SetQuickDebug(true);
	}
	return;
}

void WebsocketManager::StartQuickDebug(Chart* chart)
{
#ifndef BUILD_WASM
	auto it = quick_debug_map_.find(chart->Name());
	if (it == quick_debug_map_.end())
	{
		return;
	}
	chart->StartDebug();
	chart_map_.insert({ chart, it->second });
	for (auto hdl : it->second)
	{
		try
		{
			server_.send(hdl, debugger_->ChartInfo(chart), websocketpp::frame::opcode::TEXT);
		}
		catch (std::exception e)
		{
			RemoveHdl(chart_map_[chart], hdl);
			if (chart_map_[chart].size() == 0)
			{
				chart->StopDebug();
				chart_map_.erase(chart_map_.find(chart));
			}
			ASYNCFLOW_WARN("[deubg] websocket send error for chart {} in quick debug, the reason is {}", chart->Name(), e.what());
		}
		
	}
	quick_debug_map_.erase(it);
#endif
}

void WebsocketManager::StopDebugChart(Chart* chart)
{
#ifndef BUILD_WASM
	if (chart == nullptr) return;
	auto it = chart_map_.find(chart);
	if (it == chart_map_.end())
	{
		chart->StopDebug();
		return;
	}
	SendStopData(chart, it->second);
	chart->StopDebug();
	chart_map_.erase(it);
#endif
}

void WebsocketManager::ContinueDebugChart(Chart* chart)
{
	manager_->GetAsyncManager().ContinueBreakpoint(chart);
}

void WebsocketManager::SendStopData(Chart* chart, HDL_CONTAINER &hdls)     
{
	bool data_flag = false;
	std::string str;
	if (chart->GetDebugData().size() != 0)
	{
		data_flag = true;
		str = debugger_->PrepareChartDebugData(chart);
	}
	for (auto hdl : hdls)
	{
		try
		{
			if (data_flag)
			{
				server_.send(hdl, str, websocketpp::frame::opcode::TEXT);
			}
			server_.send(hdl, debugger_->StopMessage(chart->Name()), websocketpp::frame::opcode::TEXT);
		}
		catch (std::exception e)
		{
			ASYNCFLOW_WARN("[deubg] websocket send error in chart {0}, the reason is {1}", chart->Name(), e.what());
		}

	}
}

void WebsocketManager::StopDebugChart(Chart* chart, websocketpp::connection_hdl hdl)
{
	if (chart == nullptr) return;
	auto it = chart_map_.find(chart);
	if (it == chart_map_.end())
		return;

	auto& hdl_set = it->second;
	if (!ContainsHdl(hdl_set, hdl))
		return;

	RemoveHdl(hdl_set, hdl);
	if (hdl_set.size() == 0)	//No clients are debugging the chart
	{
		chart->StopDebug();
		chart_map_.erase(it);
	}
}

void WebsocketManager::SendReply(websocketpp::connection_hdl hdl, const std::string& msg)
{
	try {
		server_.send(hdl, msg, websocketpp::frame::opcode::TEXT);
	}
	catch (std::exception e)
	{
		ASYNCFLOW_WARN("[deubg] websocket send error, the reason is {}", e.what());
	}
}

void WebsocketManager::OnMessage(websocketpp::connection_hdl hdl, WebsocketAsioServer::message_ptr msg)
{
	assert(manager_ != nullptr);
	auto& data = msg->get_payload();
	ASYNCFLOW_DBG("[debug] recv {}", data);
	WebsocketDebugConnection connect{ this, hdl };
	debugger_->HandleMessage(data, manager_, &connect);
}

bool WebsocketManager::IsPortAvailable(const char* ip, int port)
{
	//创建一个socket 尝试连接
	auto ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == -1)
	{
		ASYNCFLOW_WARN("[deubg] cannot creat socket when check port");
		return false;
	}

	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(clientService.sin_addr.s_addr));
	clientService.sin_port = htons(port);

	// connect to server
	int iResult = bind(ConnectSocket, (sockaddr *)& clientService, sizeof(clientService));
	if (iResult == 0)
	{
		ASYNCFLOW_DBG("[deubg] port {0} is not in use", port);
#ifdef _WIN32
		closesocket(ConnectSocket);
#else
		close(ConnectSocket);
#endif
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////

void WebsocketDebugConnection::StartDebugChart(Chart* chart)
{
	websocketManager_->StartDebugChart(chart, hdl_);
}

void WebsocketDebugConnection::StopDebugChart(Chart* chart)
{
	websocketManager_->StopDebugChart(chart, hdl_);
}

void WebsocketDebugConnection::QuickDebugChart(ChartData* chart_data)
{
	websocketManager_->QuickDebugChart(chart_data, hdl_);
}

void WebsocketDebugConnection::ContinueDebugChart(Chart* chart)
{
	websocketManager_->ContinueDebugChart(chart);
}

void WebsocketDebugConnection::Reply(const std::string& msg)
{
	websocketManager_->SendReply(hdl_, msg);
}

#endif
