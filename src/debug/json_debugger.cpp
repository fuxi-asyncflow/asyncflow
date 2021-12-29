#ifdef FLOWCHART_DEBUG
#include "rapidjson/document.h"
#include "debug/json_debugger.h"
#include "util/json.h"
#include "rapidjson/prettywriter.h"
#include "core/manager.h"

#ifdef _MSC_VER
#undef GetObject
#endif

using namespace asyncflow::debug;

DebugChartInfo GetDebugChartInfoFromJson(const rapidjson::Document::Object& params)
{
	auto chart_name = params["chart_name"].GetString();
	auto agent_id = params["agent_id"].GetInt();
	auto owner_node_addr = params["owner_node_addr"].GetUint64();
	return DebugChartInfo(agent_id, (Node*)owner_node_addr, nullptr, chart_name);
}

JsonDebugger::JsonDebugger()
{
	handlers_["get_chart_list"] = GetChartListHandler;
	handlers_["debug_chart"] = DebugChartHandler;
	handlers_["stop_chart"] = StopChartHandler;
	handlers_["quick_debug"] = QuickDebugHandler;
	handlers_["break_point"] = BreakPointHandler;
	handlers_["continue"] = ContinueHandler;
	handlers_["gm"] = GmHandler;
	handlers_["hotfix"] = HotFixHandler;
}


std::string JsonDebugger::GetChartList(Manager* manager, const std::string& obj_name, const std::string& chart_name, int msg_id)
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
		Serialize_ChartInfo_Json(writer, *chart_info);
		delete chart_info;
	}
	v.clear();
	writer.EndArray();
	writer.EndObject();
	if (msg_id != -1)
	{
		writer.String("id");
		writer.Int(msg_id);
	}
	writer.EndObject();
	return std::string(sb.GetString());
}

std::string JsonDebugger::ChartInitData(const std::string& method, ChartData* chart_data, int msg_id)
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
	if (msg_id != -1)
	{
		writer.String("id");
		writer.Int(msg_id);
	}
	writer.EndObject();
	return std::string(sb.GetString());
}

std::string JsonDebugger::PrepareChartDebugData(Chart* chart)
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
			Serialize_NodeStatusData_Json(writer, *reinterpret_cast<NodeStatusData*>(data));
		else if (data->type == DebugData::VariableStatus)
			Serialize_VariableStatusData_Json(writer, *reinterpret_cast<VariableStatusData*>(data));			
		else if (data->type == DebugData::EventStatus)
			Serialize_EventStatusData_Json(writer, *reinterpret_cast<EventStatusData*>(data));			
	}
	chart->ClearDebugData();
	writer.EndArray();
	writer.EndObject();
	writer.EndObject();
	return std::string(sb.GetString());
}

void JsonDebugger::ChartDataToJson(ChartData* chart_data, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
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

std::string JsonDebugger::ErrorReply(int code, const std::string& error_msg, int msg_id)
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
	if (msg_id != -1)
	{
		writer.String("id");
		writer.Int(msg_id);
	}
	writer.EndObject();
	return std::string(sb.GetString());
}

std::string JsonDebugger::SimpleReply(const std::string& method, const std::vector<std::string>& result, int msg_id)
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
	if (msg_id != -1)
	{
		writer.String("id");
		writer.Int(msg_id);
	}
	writer.EndObject();
	return std::string(sb.GetString());
}

std::string JsonDebugger::StopMessage(const std::string& chart_name)
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

std::string JsonDebugger::SimpleReply(const std::string& method, const std::unordered_map<std::string, std::string>& result, int msg_id)
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
	if (msg_id != -1)
	{
		writer.String("id");
		writer.Int(msg_id);
	}
	writer.EndObject();
	return std::string(sb.GetString());
}

std::string JsonDebugger::ChartInfo(Chart* chart)
{
	return GenChartInfo(chart);
}

std::string JsonDebugger::GenChartInfo(Chart* chart)
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

void JsonDebugger::HandleMessage(const std::string& msg, Manager* manager_, DebugConnection* connection)
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
		if (!doc.HasMember("jsonrpc"))
			return;

		int msg_id = -1;
		if (doc.HasMember("id"))
		{
			msg_id = doc["id"].GetInt();
		}

		std::string func_name = doc["method"].GetString();
		auto it = handlers_.find(func_name);
		if(it == handlers_.end())
		{
			std::stringstream ss;
			ss << "method " << func_name << " not find";
			connection->Reply(ErrorReply(-32601, ss.str().c_str(), msg_id));
			return;
		}

		auto& handler = it->second;
		handler(doc, manager_, connection, msg_id);		
	}
	catch (std::exception e)
	{
		ASYNCFLOW_WARN("[debug] OnMessage error {0}", e.what());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void asyncflow::debug::Serialize_ChartInfo_Json(Writer& writer, const ChartInfo& ci)
{
	writer.StartObject();

	writer.String("agent_id");
	writer.Int(ci.agent_id);

	writer.String("owner_node_addr");
	writer.Uint64((uint64_t)ci.owner_node_addr);

	writer.String("owner_node_id");
	writer.Int(ci.owner_node_id);

	writer.String("object_name");
	writer.String(ci.object_name.c_str());

	writer.String("chart_name");
	writer.String(ci.chart_name.c_str());

	writer.String("owner_chart_name");
	writer.String(ci.owner_chart_name.c_str());
	writer.EndObject();
}


void asyncflow::debug::Deserialize_ChartInfo_Json(const rapidjson::Value& obj, ChartInfo& ci)
{
	if (obj.HasMember("agent_id") && obj["agent_id"].IsInt())
		ci.agent_id = obj["agent_id"].GetInt();
	if (obj.HasMember("owner_node_addr") && obj["owner_node_addr"].IsInt64())
		ci.owner_node_addr = (void*)obj["owner_node_addr"].GetUint64();
	if (obj.HasMember("owner_node_id") && obj["owner_node_id"].IsInt())
		ci.owner_node_id = obj["owner_node_id"].GetInt();
	if (obj.HasMember("object_name") && obj["object_name"].IsString())
		ci.object_name = obj["object_name"].GetString();
	if (obj.HasMember("chart_name") && obj["chart_name"].IsString())
		ci.chart_name = obj["chart_name"].GetString();
	if (obj.HasMember("owner_chart_name") && obj["owner_chart_name"].IsString())
		ci.owner_chart_name = obj["owner_chart_name"].GetString();
}


void asyncflow::debug::Serialize_NodeStatusData_Json(Writer& writer, const NodeStatusData& nsd)
{
	writer.StartObject();

	writer.String("type");
	writer.String("node_status");

	writer.String("id");
	writer.Int(nsd.id);

	writer.String("node_id");
	writer.Int(nsd.node_id);

	writer.String("node_uid");
	writer.String(nsd.node_uid.c_str());

	writer.String("old_status");
	writer.Int(nsd.old_status);

	writer.String("new_status");
	writer.Int(nsd.new_status);

	if (nsd.new_status == 2)   //½ÚµãÏÖÔÚµÄ×´Ì¬ÎªÍê³É×´Ì¬·¢ËÍÔËÐÐ½á¹û
	{
		writer.String("result");
		writer.Bool(nsd.result);
	}
	writer.EndObject();
}


void asyncflow::debug::Deserialize_NodeStatusData_Json(const rapidjson::Value& obj, NodeStatusData& nsd)
{
	if (obj.HasMember("id") && obj["id"].IsInt())
		nsd.id = obj["id"].GetInt();
	if (obj.HasMember("node_id") && obj["node_id"].IsInt())
		nsd.node_id = obj["node_id"].GetInt();
	if (obj.HasMember("node_uid") && obj["node_uid"].IsString())
		nsd.node_uid = obj["node_uid"].GetString();
	if (obj.HasMember("old_status") && obj["old_status"].IsInt())
		nsd.old_status = obj["old_status"].GetInt();
	if (obj.HasMember("new_status") && obj["new_status"].IsInt())
		nsd.new_status = obj["new_status"].GetInt();
	if (obj.HasMember("result") && obj["result"].IsBool())
		nsd.result = obj["result"].GetBool();
}


void asyncflow::debug::Serialize_VariableStatusData_Json(Writer& writer, const VariableStatusData& vsd)
{
	writer.StartObject();

	writer.String("type");
	writer.String("variable_status");

	writer.String("id");
	writer.Int(vsd.id);

	writer.String("variable_name");
	writer.String(vsd.variable_name.c_str());

	writer.String("node_uid");
	writer.String(vsd.node_uid.c_str());

	writer.String("old_value");
	writer.String(vsd.old_value.c_str());

	writer.String("new_value");
	writer.String(vsd.new_value.c_str());

	writer.EndObject();
}


void asyncflow::debug::Deserialize_VariableStatusData_Json(const rapidjson::Value& obj, VariableStatusData& vsd)
{
	if (obj.HasMember("id") && obj["id"].IsInt())
		vsd.id = obj["id"].GetInt();
	if (obj.HasMember("variable_name") && obj["variable_name"].IsString())
		vsd.variable_name = obj["variable_name"].GetString();
	if (obj.HasMember("node_uid") && obj["node_uid"].IsString())
		vsd.node_uid = obj["node_uid"].GetString();
	if (obj.HasMember("old_value") && obj["old_value"].IsString())
		vsd.old_value = obj["old_value"].GetString();
	if (obj.HasMember("new_value") && obj["new_value"].IsString())
		vsd.new_value = obj["new_value"].GetString();
}



void asyncflow::debug::Serialize_EventStatusData_Json(Writer& writer, const EventStatusData& esd)
{
	writer.StartObject();

	writer.String("type");
	writer.String("event_status");

	writer.String("id");
	writer.Int(esd.id);

	writer.String("event_name");
	writer.String(esd.event_name.c_str());

	writer.String("node_uid");
	writer.String(esd.node_uid.c_str());

	writer.String("arg_count");
	writer.Int(esd.argcount);

	writer.String("event_params");
	writer.StartArray();
	for (int i = 0; i < esd.argcount; i++)
	{
		writer.String(esd.n_args[i].c_str());
	}
	writer.EndArray();

	writer.EndObject();
}

void asyncflow::debug::Deserialize_EventStatusData_Json(const rapidjson::Value& obj, EventStatusData& esd)
{
	if (obj.HasMember("id") && obj["id"].IsInt())
		esd.id = obj["id"].GetInt();
	if (obj.HasMember("event_name") && obj["event_name"].IsString())
		esd.event_name = obj["event_name"].GetString();
	if (obj.HasMember("node_uid") && obj["node_uid"].IsString())
		esd.node_uid = obj["node_uid"].GetString();
	if (obj.HasMember("arg_count") && obj["arg_count"].IsInt())
		esd.argcount = obj["arg_count"].GetInt();
	if (obj.HasMember("event_params") && obj["event_params"].IsArray())
	{
		auto array = obj["event_params"].GetArray();
		esd.n_args.clear();
		for (const auto& arg : array)
		{
			if (arg.IsString())
				esd.n_args.push_back(arg.GetString());
		}
	}
}

std::string JsonDebugger::HeartBeat()
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.String("jsonrpc");
	writer.String("2.0");
	writer.String("method");
	writer.String("heart_beat");
	writer.EndObject();
	return std::string(sb.GetString());
}

void JsonDebugger::GetChartListHandler(rapidjson::Document& doc, Manager* manager_, DebugConnection* connection, int msg_id)
{
	auto params = doc["params"].GetObject();
	const auto* chart_name = params["chart_name"].GetString();
	const auto* obj_name = params["obj_name"].GetString();
	if (chart_name == nullptr)
		chart_name = "";
	if (obj_name == nullptr)
		obj_name = "";
	connection->Reply(GetChartList(manager_, chart_name, obj_name, msg_id));
}

void JsonDebugger::DebugChartHandler(rapidjson::Document& doc, Manager* manager_, DebugConnection* connection, int msg_id)
{
	auto params = doc["params"].GetObject();
	auto chartInfo = GetDebugChartInfoFromJson(params);
	auto msg = FindChart(manager_, chartInfo);
	auto* chart = chartInfo.chart;
	if (chart == nullptr)
	{
		connection->Reply(ErrorReply(-1, msg, msg_id));
	}
	else
	{
		connection->Reply(ChartInitData("debug_chart", chart->GetData(), msg_id));
		connection->StartDebugChart(chart);
	}
}

void JsonDebugger::StopChartHandler(rapidjson::Document& doc, Manager* manager_, DebugConnection* connection, int msg_id)
{
	auto params = doc["params"].GetObject();
	auto chartInfo = GetDebugChartInfoFromJson(params);
	auto msg = FindChart(manager_, chartInfo);
	auto* chart = chartInfo.chart;
	if (chart == nullptr)
	{
		connection->Reply(ErrorReply(-1, msg, msg_id));
	}
	else
	{
		std::unordered_map<std::string, std::string> result;
		result["chart_name"] = chartInfo.chart_name;
		connection->Reply(SimpleReply("stop_chart", result, msg_id));
		connection->StopDebugChart(chart);
	}
}

void JsonDebugger::QuickDebugHandler(rapidjson::Document& doc, Manager* manager_, DebugConnection* connection, int msg_id)
{
	auto params = doc["params"].GetObject();
	auto chart_name = params["chart_name"].GetString();
	auto chart_list = manager_->GetChartsByName(chart_name);
	//Run the first chart in chart list by default
	if (chart_list.size() > 0)
	{
		auto chart = chart_list.front();
		connection->Reply(ChartInitData("quick_debug", chart->GetData(), msg_id));
		connection->Reply(GenChartInfo(chart));
		connection->StartDebugChart(chart);
		return;
	}
	auto chart_data = manager_->GetChartData(chart_name);
	if (chart_data == nullptr)
	{
		connection->Reply(ErrorReply(-10, "The chart is not exist in chart_data", msg_id));
	}
	else
	{
		connection->QuickDebugChart(chart_data);
		connection->Reply(ChartInitData("quick_debug", chart_data, msg_id));
	}
}

void JsonDebugger::BreakPointHandler(rapidjson::Document& doc, Manager* manager_, DebugConnection* connection, int msg_id)
{
	auto params = doc["params"].GetObject();
	auto chart_name = params["chart_name"].GetString();
	auto chart_data = manager_->GetChartData(chart_name);
	if (chart_data == nullptr)
	{
		connection->Reply(ErrorReply(-10, "The chart is not exist in chart_data", msg_id));
		return;
	}
	auto uid = params["node_uid"].GetString();
	auto node_data = chart_data->GetNodeData(uid);
	if (node_data == nullptr)
	{
		connection->Reply(ErrorReply(-4, "The uid is not exist in chart_data", msg_id));
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
		connection->Reply(SimpleReply("break_point", result, msg_id));
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
		connection->Reply(SimpleReply("break_point", result, msg_id));
	}
}

void JsonDebugger::ContinueHandler(rapidjson::Document& doc, Manager* manager_, DebugConnection* connection, int msg_id)
{
	auto params = doc["params"].GetObject();
	auto chartInfo = GetDebugChartInfoFromJson(params);
	auto msg = FindChart(manager_, chartInfo);
	auto* chart = chartInfo.chart;
	if (chart == nullptr)
	{
		connection->Reply(ErrorReply(-1, msg, msg_id));
	}
	else
	{
		std::unordered_map<std::string, std::string> result;
		result["chart_name"] = chartInfo.chart_name;
		connection->Reply(SimpleReply("continue", result, msg_id));
		connection->ContinueDebugChart(chart);
	}
}

void JsonDebugger::GmHandler(rapidjson::Document& doc, Manager* manager_, DebugConnection* connection, int msg_id)
{
	auto params = doc["params"].GetObject();
	auto script = params["script"].GetString();
	auto result = manager_->RunScript(script);
	if (result.first)
	{
		connection->Reply(SimpleReply("gm", result.second, msg_id));
	}
	else
		connection->Reply(ErrorReply(-10, result.second.front(), msg_id));
}

void JsonDebugger::HotFixHandler(rapidjson::Document& doc, Manager* manager_, DebugConnection* connection, int msg_id)
{
	auto params = doc["params"].GetObject();
	auto charts_func = params["charts_func"].GetString();
	auto result = manager_->RunScript(charts_func);
	if (!result.first)
	{
		connection->Reply(ErrorReply(-15, "run charts_func error", msg_id));
		return;
	}
	auto* charts_data_str = params["charts_data"].GetString();
	auto chart_data_list = manager_->ParseChartsFromJson(charts_data_str);
	manager_->ImportChatData(chart_data_list);
	for (auto* data : chart_data_list)
	{
		manager_->RestartChart(data->Name());
	}

	if (chart_data_list.empty())
	{
		connection->Reply(ErrorReply(-15, "import charts_data error", msg_id));
		return;
	}
	connection->Reply(SimpleReply("hotfix", std::vector<std::string>(), msg_id));
}

#endif