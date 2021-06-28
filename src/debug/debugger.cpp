#ifdef FLOWCHART_DEBUG
#include "debug/debugger.h"
#include "util/json.h"
#include "rapidjson/prettywriter.h"
#include "util/log.h"
using namespace asyncflow::debug;

#ifdef _MSC_VER
#undef GetObject
#endif

std::string Debugger::GetChartList(Manager* manager, const std::string& obj_name, const std::string& chart_name, int id)
{
	auto v = manager->GetDebugChartList(chart_name, obj_name);

	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

	writer.StartObject();
	writer.String("jsonrpc");
	writer.String("2.0");
	writer.String("method");
	writer.String("get_chart_list");

	writer.String("result");
	writer.StartObject();
	writer.String("chart_info");
	writer.StartArray();
	for (auto* chart_info : v)
	{
		chart_info->Serialize(writer);
		delete chart_info;
	}
	v.clear();
	writer.EndArray();
	writer.EndObject();
	if (id != -1)
	{
		writer.String("id");
		writer.Int(id);
	}
	writer.EndObject();
	return std::string(sb.GetString());
}

std::string Debugger::FindChart(core::Manager* manager, int agent_id, const std::string& chart_name, core::Node* owner_node, core::Chart** pchart)
{
	auto* agent = manager->GetAgentManager().GetAgentById(agent_id);
	//ensure agent is not destroyed
	if (agent == nullptr)
	{
		ASYNCFLOW_WARN("[debug] agent {0} does not exist", agent_id);
		std::string error_msg = "agent_id " + std::to_string(agent_id) + " not exist";
		return error_msg;
	}
	auto chart = agent->FindChart(chart_name, owner_node);
	if (chart == nullptr)
	{
		ASYNCFLOW_WARN("[debug] Chart {0} with agent id {1} ownernode {2} does not exist", chart_name, agent_id, (void*)owner_node);
		std::string error_msg = "chart_name " + chart_name + " does not exist";
		return error_msg;
	}
	*pchart = chart;
	return "";

}

std::string Debugger::DebugChart(Manager* manager, int agent_id, const std::string& chart_name, Node* owner_node, Chart** pchart, int id)
{
	const auto reply = FindChart(manager, agent_id, chart_name, owner_node, pchart);
	//ensure agent is not destroyed
	if (pchart == nullptr)
	{
		return ErrorReply(-1, reply, id);
	}
	else
	{
		ASYNCFLOW_DBG("[debug] DebugChart {0} {1} {2}", agent_id, chart_name, (void*)owner_node);
		return ChartInitData("debug_chart", (*pchart)->GetData(), id);
	}
}

std::string Debugger::ContinueDebug(Manager* manager, int agent_id, const std::string& chart_name, Node* owner_node, Chart** pchart, int id)
{
	ASYNCFLOW_DBG("[debug] ContinueChart {0} {1} {2}", agent_id, chart_name, (void*)owner_node);
	const auto reply = FindChart(manager, agent_id, chart_name, owner_node, pchart);
	if (pchart == nullptr)
	{
		return ErrorReply(-1, reply, id);
	}
	else
	{
		std::unordered_map<std::string, std::string> result;
		result["chart_name"] = chart_name;
		return SimpleReply("continue", result, id);
	}
}

std::string Debugger::StopChart(Manager* manager, int agent_id, const std::string& chart_name, Node* owner_node, Chart** pchart, int id)
{
	ASYNCFLOW_DBG("[debug] StopDebugChart {0} {1} {2}", agent_id, chart_name, (void*)owner_node);
	auto reply = FindChart(manager, agent_id, chart_name, owner_node, pchart);
	if (pchart == nullptr)
	{
		return ErrorReply(-1, reply, id);
	}
	else
	{
		std::unordered_map<std::string, std::string> result;
		result["chart_name"] = chart_name;
		return SimpleReply("stop_chart", result, id);
	}
}

std::string Debugger::ChartInitData(const std::string& method, ChartData* chart_data, int id)
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.String("jsonrpc");
	writer.String("2.0");
	writer.String("method");
	writer.String(method.c_str());
	writer.String("result");
	writer.StartObject();
	writer.String("chart_name");
	writer.String(chart_data->Name().c_str());
	writer.String("chart_data");
	ChartDataToJson(chart_data, writer);
	writer.EndObject();
	if (id != -1)
	{
		writer.String("id");
		writer.Int(id);
	}
	writer.EndObject();
	return std::string(sb.GetString());
}

std::string Debugger::PrepareChartDebugData(Chart* chart)
{
	if (chart->GetDebugData().size() == 0)
		return "";
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.String("jsonrpc");
	writer.String("2.0");
	writer.String("method");
	writer.String("debug_chart");
	writer.String("params");
	writer.StartObject();
	writer.String("chart_name");
	writer.String(chart->Name().c_str());
	writer.String("running_data");
	writer.StartArray();
	for (debug::DebugData* data : chart->GetDebugData())
	{
		if (data->type == DebugData::NodeStatus)
			reinterpret_cast<NodeStatusData*>(data)->Serialize(writer);
		else if (data->type == DebugData::VariableStatus)
			reinterpret_cast<VariableStatusData*>(data)->Serialize(writer);
		else if (data->type == DebugData::EventStatus)
			reinterpret_cast<EventStatusData*>(data)->Serialize(writer);
	}
	chart->ClearDebugData();
	writer.EndArray();
	writer.EndObject();
	writer.EndObject();
	return std::string(sb.GetString());
}

void Debugger::ChartDataToJson(ChartData* chart_data, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("nodes");
	writer.StartArray();
	for (int i = 0; i < chart_data->GetNodeCount(); i++)
	{
		auto node = chart_data->GetNodeData(i);
		writer.StartObject();
		writer.String("uid");
		writer.String(node->GetUid().c_str());
		writer.String("text");
		writer.String(node->GetText().c_str());
		writer.EndObject();
	}
	writer.EndArray();

	writer.String("edges");
	writer.StartArray();
	for (int i = 0; i < chart_data->GetNodeCount(); i++)
	{
		auto node = chart_data->GetNodeData(i);
		for (auto child : node->GetSubsequenceIds(false))
		{
			writer.StartObject();
			writer.String("parent");
			writer.String(node->GetUid().c_str());
			writer.String("child");
			writer.String(chart_data->GetNodeData(child)->GetUid().c_str());
			writer.EndObject();
		}

		for (auto child : node->GetSubsequenceIds(true))
		{
			writer.StartObject();
			writer.String("parent");
			writer.String(node->GetUid().c_str());
			writer.String("child");
			writer.String(chart_data->GetNodeData(child)->GetUid().c_str());
			writer.EndObject();
		}
	}
	writer.EndArray();

	writer.EndObject();
	return;
}

std::string Debugger::ErrorReply(int code, const std::string& error_msg, int id)
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

	writer.StartObject();
	writer.String("error");
	writer.StartObject();
	writer.String("code");
	writer.Int(code);
	writer.String("message");
	writer.String(error_msg.c_str());
	writer.EndObject();
	if (id != -1)
	{
		writer.String("id");
		writer.Int(id);
	}
	writer.EndObject();
	return std::string(sb.GetString());
}

std::string Debugger::NormalMessage(const std::string& method, const std::vector<std::string>& result, int id)
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.String("jsonrpc");
	writer.String("2.0");
	writer.String("method");
	writer.String(method.c_str());
	writer.String("result");
	writer.StartArray();
	for (auto& str : result)
	{
		writer.String(str.c_str());
	}
	writer.EndArray();
	if (id != -1)
	{
		writer.String("id");
		writer.Int(id);
	}
	writer.EndObject();
	return std::string(sb.GetString());
}

std::string Debugger::StopMessage(const std::string& chart_name)
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.String("jsonrpc");
	writer.String("2.0");
	writer.String("method");
	writer.String("stop_chart");
	writer.String("params");
	writer.StartObject();
	writer.String("chart_name");
	writer.String(chart_name.c_str());
	writer.EndObject();
	writer.EndObject();
	return std::string(sb.GetString());
}

std::string Debugger::SimpleReply(const std::string& method, const std::unordered_map<std::string, std::string>& result, int id)
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.String("jsonrpc");
	writer.String("2.0");
	writer.String("method");
	writer.String(method.c_str());
	writer.String("result");
	writer.StartObject();
	for (const auto& pair : result)
	{
		writer.String(pair.first.c_str());
		writer.String(pair.second.c_str());
	}
	writer.EndObject();
	if (id != -1)
	{
		writer.String("id");
		writer.Int(id);
	}
	writer.EndObject();
	return std::string(sb.GetString());
}

std::string Debugger::ChartInfo(Chart* chart)
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.String("jsonrpc");
	writer.String("2.0");
	writer.String("method");
	writer.String("quick_debug");
	writer.String("params");
	writer.StartObject();
	writer.String("chart_name");
	writer.String(chart->Name().c_str());
	writer.String("agent_id");
	writer.Int(chart->GetAgent()->GetId());
	writer.String("owner_node_addr");
	writer.Uint64((uint64_t)chart->GetOwnerNode());
	writer.EndObject();
	writer.EndObject();
	return std::string(sb.GetString());
}

void Debugger::HandleMessage(const std::string& msg, Manager* manager_, HandleObject* obj)
{
	rapidjson::Document doc;
	bool valid_json = util::JsonUtil::ParseJson(msg, doc);
	if (!valid_json)
	{
		ASYNCFLOW_WARN("[debug] json parse error");
		return;
	}
	try
	{
		if (doc.HasMember("jsonrpc"))
		{
			std::string func_name = doc["method"].GetString();
			auto params = doc["params"].GetObject();
			int id = -1;
			if (doc.HasMember("id"))
			{
				id = doc["id"].GetInt();
			}
			if (func_name == "get_chart_list")
			{
				auto chart_name = params["chart_name"].GetString();
				auto obj_name = params["obj_name"].GetString();
				if (chart_name == nullptr)
					chart_name = "";
				if (obj_name == nullptr)
					obj_name = "";
				obj->Reply(Debugger::GetChartList(manager_, chart_name, obj_name, id));
			}
			else if (func_name == "debug_chart")
			{
				auto chart_name = params["chart_name"].GetString();
				auto agent_id = params["agent_id"].GetInt();
				auto owner_node_addr = params["owner_node_addr"].GetUint64();
				Chart* chart = nullptr;
				obj->Reply(DebugChart(manager_, agent_id, chart_name, (core::Node*)owner_node_addr, &chart, id));
				if (chart != nullptr)
				{
					obj->StartDebugChart(chart);
				}
			}
			else if (func_name == "stop_chart")
			{
				auto chart_name = params["chart_name"].GetString();
				auto agent_id = params["agent_id"].GetInt();
				auto owner_node_addr = params["owner_node_addr"].GetUint64();
				Chart* chart = nullptr;
				obj->Reply(StopChart(manager_, agent_id, chart_name, (core::Node*)owner_node_addr, &chart, id));
				if (chart != nullptr)
				{
					obj->StopDebugChart(chart);
				}
			}
			else if (func_name == "quick_debug")
			{
				auto chart_name = params["chart_name"].GetString();
				auto chart_list = manager_->GetChartsByName(chart_name);
				//Run the first chart in chart list by default
				if (chart_list.size() > 0)
				{
					auto chart = chart_list.front();
					obj->Reply(ChartInitData("quick_debug", chart->GetData(), id));
					obj->Reply(ChartInfo(chart));
					obj->StartDebugChart(chart);
					return;
				}
				auto chart_data = manager_->GetChartData(chart_name);
				if (chart_data == nullptr)
				{
					obj->Reply(ErrorReply(-10, "The chart is not exist in chart_data", id));
				}
				else
				{
					obj->QuickDebugChart(chart_data);
					obj->Reply(ChartInitData("quick_debug", chart_data, id));
				}
			}
			else if (func_name == "break_point")
			{
				auto chart_name = params["chart_name"].GetString();
				auto chart_data = manager_->GetChartData(chart_name);
				if (chart_data == nullptr)
				{
					obj->Reply(ErrorReply(-10, "The chart is not exist in chart_data", id));
					return;
				}
				auto uid = params["node_uid"].GetString();
				auto node_data = chart_data->GetNodeData(uid);
				if (node_data == nullptr)
				{
					obj->Reply(ErrorReply(-4, "The uid is not exist in chart_data", id));
					return;
				}
				auto command = params["command"].GetString();
				std::unordered_map<std::string, std::string> result;
				result["chart_name"] = chart_name;
				result["node_uid"] = uid;
				if (strcmp(command, "set") == 0)
				{
					if (!node_data->IsBreakPoint())
					{
						node_data->SetBreakPoint(true);
						manager_->SetBreakpoint(node_data);
					}
					result["command"] = "set";
					obj->Reply(SimpleReply("break_point", result, id));
				}
				else
				{
					assert(strcmp(command, "delete") == 0);
					if (node_data->IsBreakPoint())
					{
						node_data->SetBreakPoint(false);
						manager_->DeleteBreakpoint(node_data);
					}
					result["command"] = "delete";
					obj->Reply(SimpleReply("break_point", result, id));
				}
			}
			else if (func_name == "continue")
			{
				auto chart_name = params["chart_name"].GetString();
				auto agent_id = params["agent_id"].GetInt();
				auto owner_node_addr = params["owner_node_addr"].GetUint64();
				Chart* chart = nullptr;
				obj->Reply(ContinueDebug(manager_, agent_id, chart_name, (core::Node*)owner_node_addr, &chart, id));
				if (chart != nullptr)
				{
					obj->ContinueDebugChart(chart);
				}
			}
			else if (func_name == "gm")
			{
				auto script = params["script"].GetString();
				auto result = manager_->RunScript(script);
				if (result.first)
				{
					obj->Reply(NormalMessage("gm", result.second, id));
				}
				else
					obj->Reply(ErrorReply(-10, result.second.front(), id));
			}
			else if (func_name == "hotfix")
			{
				auto charts_func = params["charts_func"].GetString();
				auto result = manager_->RunScript(charts_func);
				if (!result.first)
				{
					obj->Reply(ErrorReply(-15, "run charts_func error", id));
					return;
				}
				auto charts_data = params["charts_data"].GetString();
				auto import_res = manager_->ImportJson(charts_data);
				if (!import_res)
				{
					obj->Reply(ErrorReply(-15, "import charts_data error", id));
					return;
				}
				obj->Reply(NormalMessage("hotfix", std::vector<std::string>(), id));
			}

			else
			{
				obj->Reply(ErrorReply(-32601, "method not find", id));
			}
		}
	}
	catch (std::exception e)
	{
		ASYNCFLOW_WARN("[debug] OnMessage error {0}", e.what());
	}
}
#endif