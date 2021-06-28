#ifdef FLOWCHART_DEBUG
#include "js_debugger.h"
#include "export_js.h"
#include "util/log.h"
#include <algorithm>
using namespace asyncflow::debug;

void JsDebugger::StartDebugChart(core::Chart* chart)
{
	if (chart == nullptr)
		return;

	auto it = std::find(charts_.begin(), charts_.end(), chart);
	if (it != charts_.end())
	{
		return;
	}
	charts_.push_back(chart);
	chart->StartDebug();
}

void JsDebugger::StopDebugChart(core::Chart* chart)
{
	if (chart == nullptr)
		return;

	auto it = std::find(charts_.begin(), charts_.end(), chart);
	if (it == charts_.end())
	{
		return;
	}
	charts_.erase(it);
}

void JsDebugger::SendStopData(core::Chart* chart)
{
	bool data_flag = false;
	std::string str;
	if (chart->GetDebugData().size() != 0)
	{
		data_flag = true;
		str = Debugger::PrepareChartDebugData(chart);
	}
	if (data_flag)
	{
		js_debug_data(str.c_str());
	}
	js_debug_data(Debugger::StopMessage(chart->Name()).c_str());
	chart->StopDebug();
}

void JsDebugger::StopDebugChartWithData(core::Chart* chart)
{
	if (chart == nullptr)
		return;

	auto it = std::find(charts_.begin(), charts_.end(), chart);
	if (it == charts_.end())
	{
		return;
	}
	charts_.erase(it);
	SendStopData(chart);
}

void JsDebugger::QuickDebugChart(core::ChartData* chart_data)
{
	auto chart_name = chart_data->Name();
	auto it = std::find(quick_debug_charts_.begin(), quick_debug_charts_.end(), chart_name);
	if (it != quick_debug_charts_.end())
	{
		return;
	}
	quick_debug_charts_.push_back(chart_name);
	chart_data->SetQuickDebug(true);
}

void JsDebugger::ContinueDebugChart(core::Chart* chart)
{
	chart->GetAgent()->GetManager()->GetAsyncManager().ContinueBreakpoint(chart);
}

void JsDebugger::Reply(const std::string& msg)
{
	js_debug_data(msg.c_str());
}

void JsDebugger::Step()
{
	ASYNCFLOW_DBG("[deubg] debugging chart count {0}", charts_.size());
	//send data
	std::vector<Chart*> check_chart;
	for (auto& chart : charts_)
	{
		auto str = Debugger::PrepareChartDebugData(chart);
		ASYNCFLOW_DBG("[deubg] chart json {0}", str);
		if (str == "")
			continue;
		js_debug_data(str.c_str());
	}
}

void JsDebugger::StartQuickDebug(core::Chart* chart)
{
	auto it = std::find(quick_debug_charts_.begin(), quick_debug_charts_.end(), chart->Name());
	if (it == quick_debug_charts_.end())
	{
		return;
	}
	charts_.push_back(chart);
	chart->StartDebug();
	js_debug_data(Debugger::ChartInfo(chart).c_str());
	quick_debug_charts_.erase(it);
}
#endif