#ifdef FLOWCHART_DEBUG
#include "debug/websocket_debugger.h"
using namespace asyncflow::debug;

void WebsocketHandleObject::StartDebugChart(core::Chart* chart)
{
	websocketManager_->StartDebugChart(chart, hdl_);
}

void WebsocketHandleObject::StopDebugChart(core::Chart* chart) 
{
	websocketManager_->StopDebugChart(chart, hdl_);
}

void WebsocketHandleObject::QuickDebugChart(core::ChartData* chart_data)
{
	websocketManager_->QuickDebugChart(chart_data, hdl_);
}

void WebsocketHandleObject::ContinueDebugChart(core::Chart* chart)
{
	websocketManager_->ContinueDebugChart(chart);
}

void WebsocketHandleObject::Reply(const std::string& msg) 
{
	websocketManager_->SendReply(hdl_, msg);
}

#endif