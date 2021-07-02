#ifdef FLOWCHART_DEBUG
#include <fstream>
#include <thread>
#include "export_lua.h"
#include "lua_manager.h"
#include "lua_chart_builder.h"
#include "util/json.h"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include "debug/debug_common.h"

#include "catch.hpp"

typedef websocketpp::client<websocketpp::config::asio_client> client;


using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

const char* get_list = "{ \"jsonrpc\": \"2.0\", \"method\": \"get_chart_list\", \"params\" : { \"chart_name\":\"\", \"obj_name\":\"\" }, \"id\":1}";
const char* quick_debug = "{\"jsonrpc\": \"2.0\",\"method\": \"quick_debug\",\"params\": {\"chart_name\": \"test_websocket\"}, \"id\": 4 }";
const char* gm_str = "{\"jsonrpc\": \"2.0\",\"method\": \"gm\",\"params\": {\"script\":\"return agent:get_charts()[1], 111\"}, \"id\": 1 }";
const char* set_breakpoint = "{\"jsonrpc\": \"2.0\",\"method\": \"break_point\",\"params\": {\"chart_name\":\"test_websocket\",\"command\":\"set\",\"node_uid\": \"00000\"},\"id\":5}";
const char* delete_breakpoint = "{\"jsonrpc\": \"2.0\",\"method\": \"break_point\",\"params\": {\"chart_name\":\"test_websocket\",\"command\":\"delete\",\"node_uid\": \"00000\"},\"id\":6}";
bool runflag;
int message_count;
const int THREAD_TOTAL_TIME = 50; // milliseconds

void run_step()
{
	for (int i = 0; i < THREAD_TOTAL_TIME / 10; i++)
	{
		dostring(L, "asyncflow.step(100)");
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void run_web(client* c) {
	for(int i=0; i< THREAD_TOTAL_TIME / 10; i++)
	{
		if (c->stopped())
			c->reset();
		c->poll();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
	rapidjson::Document doc;
	bool valid_json = asyncflow::util::JsonUtil::ParseJson(msg->get_payload(), doc);
	if (!valid_json)
	{
		REQUIRE(false);
	}
	REQUIRE(strcmp(doc["method"].GetString(), "get_chart_list") == 0);
	REQUIRE(doc["id"].GetInt() == 1);
	REQUIRE(doc["result"].IsObject());
	auto params = doc["result"].GetObject();
	REQUIRE(params.HasMember("chart_info"));
	REQUIRE(params["chart_info"].IsArray());
	auto charts = params["chart_info"].GetArray();
	REQUIRE(charts.Size() == 1);

	asyncflow::debug::ChartInfo chart_info;
	chart_info.Deserialize(charts[0]);
	REQUIRE(chart_info.owner_node_addr == 0);
	REQUIRE(chart_info.owner_node_id == -1);
	REQUIRE(chart_info.chart_name == "test_websocket");
	REQUIRE(chart_info.owner_chart_name == "");
	//std::cout << msg->get_payload() << std::endl;
	runflag = false;
}

void on_open(client* c, const char* str, websocketpp::connection_hdl hdl)
{
	try
	{
		c->send(hdl, str, websocketpp::frame::opcode::text);
	}
	catch (websocketpp::exception const& ec)
	{
		ASYNCFLOW_ERR("send message error in on_open handle because: {0} ", ec.what());
		runflag = false;
	}
}

asyncflow::lua::LuaChartData* get_simple_chartdata(lua_State* L)
{
	auto builder = LuaChartBuilder(L);
	auto* node_0 = builder.AddNode([&]()
	{
		return 0;
	});

	auto* node_1 = builder.AddNode([&]()
	{
		return 0;
	});

	node_0->uid_ = "00000";
	node_1->uid_ = "00001";
	node_0->RunBefore(node_1);
	node_1->RunBefore(node_0);
	node_0->is_orphan = true;
	auto* chart_data = builder.Build();
	chart_data->SetName("test_websocket");
	return chart_data;
}

TEST_CASE("get list test")
{
	INIT_LUA_TEST
	////////////////////////////////////////////////////////////////////////////////////////////	
	auto* chart_data = get_simple_chartdata(L);
	agent->AttachChart(chart_data);
	agent->Start();
	client c;
	std::string url = "ws://localhost:9000";
	c.init_asio();
	c.clear_access_channels(websocketpp::log::alevel::all);
	c.set_message_handler(bind(&on_message, &c, ::_1, ::_2));
	c.set_open_handler(bind(&on_open, &c, get_list, ::_1));
	websocketpp::lib::error_code ec;
	auto hdl = c.get_connection(url, ec);
	if (ec)
	{
		ASYNCFLOW_ERR("could not create connection because: {} ", ec.message());
		runflag = false;
	}
	c.connect(hdl);
	runflag = true;
	std::thread t1([&]() {run_step(); });
	std::thread t2([&c]() {run_web(&c); });
	t1.join();
	t2.join();

	////////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}

void debug_chart_on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
	rapidjson::Document doc;
	bool valid_json = asyncflow::util::JsonUtil::ParseJson(msg->get_payload(), doc);
	if (!valid_json)
	{
		REQUIRE(false);
	}
	//std::cout << msg->get_payload() << std::endl;
	REQUIRE(strcmp(doc["method"].GetString(), "debug_chart") == 0);
	if (message_count == 0)
	{
		REQUIRE(doc["id"].GetInt() == 2);
		REQUIRE(doc["result"]["chart_name"] == "test_debug_chart");
		auto data = doc["result"]["chart_data"].GetObject();
		REQUIRE(data.HasMember("nodes"));
		REQUIRE(data.HasMember("edges"));
	}
	else if (message_count != 0 && message_count < 5)
	{
		REQUIRE(doc["params"]["chart_name"] == "test_debug_chart");
		auto data = doc["params"]["running_data"].GetArray();
		REQUIRE(data.Size() == 6);
		REQUIRE(data[0]["type"] == "event_status");
		asyncflow::debug::EventStatusData event_data;
		event_data.Deserialize(data[0]);
		REQUIRE(event_data.event_name == "Event2Arg");
		REQUIRE(event_data.argcount == 2);
		REQUIRE(event_data.n_args[0] == "first");
		REQUIRE(event_data.n_args[1] == "second");
		REQUIRE(data[1]["type"] == "node_status");
		asyncflow::debug::NodeStatusData node_data;
		node_data.Deserialize(data[1]);
		REQUIRE(node_data.old_status == 0);
		REQUIRE(node_data.new_status == 2);
		REQUIRE(data[2]["type"] == "variable_status");
		asyncflow::debug::VariableStatusData variable_data;
		variable_data.Deserialize(data[2]);
		REQUIRE(variable_data.old_value == "hello");
		REQUIRE(variable_data.new_value == "ss111");
	}
	else
	{
		runflag = false;
	}
	++message_count;
}

std::string command_str(const std::string& str, const std::string& chart_name, int agent_id, int id)
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.String("jsonrpc");
	writer.String("2.0");
	writer.String("method");
	writer.String(str.c_str());
	writer.String("params");
	writer.StartObject();
	writer.String("agent_id");
	writer.Int(agent_id);
	writer.String("chart_name");
	writer.String(chart_name.c_str());
	writer.String("owner_node_addr");
	writer.Int64(0);
	writer.EndObject();
	writer.String("id");
	writer.Int(id);
	writer.EndObject();
	return std::string(sb.GetString());
}

void run_event(Agent* agent)
{
	for (int i = 0; i < THREAD_TOTAL_TIME / 10; i++)
	{
		manager->Step(10);
		lua_pushstring(L, "first");
		int arg_ref = luaL_ref(L, LUA_REGISTRYINDEX);
		int* args = new int[2];
		args[0] = arg_ref;
		lua_pushstring(L, "second");
		arg_ref = luaL_ref(L, LUA_REGISTRYINDEX);
		args[1] = arg_ref;
		manager->Manager::Event(5, agent, args, 2, false);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

TEST_CASE("debug chart test")
{
	INIT_LUA_TEST_EVENT
	////////////////////////////////////////////////////////////////////////////////////////////	
	auto builder = LuaChartBuilder(L);
	int count = 0;
	auto* node_0 = builder.AddNode([&]()
	{
		lua_settop(L, 0);
		lua_pushstring(L, "hello");
		manager->SetVar(L, 0);
		return 0;
	});

	auto* node_1 = builder.AddNode([&]()
	{
		manager->WaitEvent(agent, 5);
		return 0;
	});
	node_1->event_id = 5;

	auto* node_2 = builder.AddNode([&agent]()
	{
		lua_settop(L, 0);
		lua_pushstring(L, "ss111");
		manager->SetVar(L, 0);
		return 0;
	});

	node_0->RunBefore(node_1);
	node_1->RunBefore(node_2);
	node_2->RunBefore(node_0);
	node_0->is_orphan = true;

	////////////////////////////////////////////////////////////////////////////////////////////
	auto* chart_data = builder.Build();
	chart_data->SetName("test_debug_chart");
	chart_data->SetVarCount(1);
	agent->AttachChart(chart_data);
	agent->Start();
	client c;
	std::string url = "ws://localhost:9000";
	c.init_asio();
	c.clear_access_channels(websocketpp::log::alevel::all);
	c.set_message_handler(bind(&debug_chart_on_message, &c, ::_1, ::_2));
	auto debug_str = command_str("debug_chart", "test_debug_chart", agent->GetId(), 2);
	c.set_open_handler(bind(&on_open, &c, debug_str.c_str(), ::_1));
	websocketpp::lib::error_code ec;
	auto hdl = c.get_connection(url, ec);
	if (ec)
	{
		ASYNCFLOW_ERR("could not create connection because: {} ", ec.message());
		runflag = false;
	}
	c.connect(hdl);
	runflag = true;
	message_count = 0;
	std::thread t1([&agent]() {run_event(agent); });
	std::thread t2([&c]() {run_web(&c); });
	t1.join();
	t2.join();

	////////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}

void stop_chart_on_message(client* c, Agent* agent, websocketpp::connection_hdl hdl, client::message_ptr msg) {
	rapidjson::Document doc;
	bool valid_json = asyncflow::util::JsonUtil::ParseJson(msg->get_payload(), doc);
	if (!valid_json)
	{
		REQUIRE(false);
	}
	//std::cout << msg->get_payload() << std::endl;
	auto* chart = agent->FindChart("test_websocket", nullptr);
	auto& map = manager->GetWebsocketManager().GetChartMap();
	if (message_count == 0)
	{
		REQUIRE(strcmp(doc["method"].GetString(), "debug_chart") == 0);
		REQUIRE(chart->IsDebug());
		auto it = map.find(chart);
		REQUIRE(it != map.end());
		REQUIRE(it->second.size() == 1);
	}
	else if (message_count == 5)
	{
		try
		{
			auto str = command_str("stop_chart", "test_websocket", agent->GetId(), 3);
			c->send(hdl, str, websocketpp::frame::opcode::text);
		}
		catch (websocketpp::exception const& ec)
		{
			ASYNCFLOW_ERR("send message error in stop_chart because: {0} ", ec.what());
			runflag = false;
		}
	}
	if (strcmp(doc["method"].GetString(), "stop_chart") == 0)
	{
		REQUIRE(strcmp(doc["result"]["chart_name"].GetString(), "test_websocket") == 0);
		REQUIRE(chart->IsDebug() == false);
		auto it = map.find(chart);
		REQUIRE(it == map.end());
		runflag = false;
	}
	++message_count;
}

TEST_CASE("stop chart test")
{
	INIT_LUA_TEST
	////////////////////////////////////////////////////////////////////////////////////////////	
	auto* chart_data = get_simple_chartdata(L);
	agent->AttachChart(chart_data);
	agent->Start();
	client c;
	std::string url = "ws://localhost:9000";
	c.init_asio();
	c.clear_access_channels(websocketpp::log::alevel::all);
	c.set_message_handler(bind(&stop_chart_on_message, &c, agent, ::_1, ::_2));
	auto debug_str = command_str("debug_chart", "test_websocket", agent->GetId(), 3);
	c.set_open_handler(bind(&on_open, &c, debug_str.c_str(), ::_1));
	websocketpp::lib::error_code ec;
	auto hdl = c.get_connection(url, ec);
	if (ec)
	{
		ASYNCFLOW_ERR("could not create connection because: {} ", ec.message());
		runflag = false;
	}
	c.connect(hdl);
	runflag = true;
	message_count = 0;
	std::thread t1([&]() {run_step(); });
	std::thread t2([&c]() {run_web(&c); });
	t1.join();
	t2.join();

	////////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}

void run_and_attach(Agent* agent)
{
	int frame = 0;
	for (int i = 0; i < THREAD_TOTAL_TIME / 10; i++)
	{
		manager->Step(10);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (frame == 150)
		{
			manager->Manager::AttachChart(agent, "test_websocket");
			agent->Start();
		}
		++frame;
	}
}

void quick_debug_on_message(client* c, Agent* agent, websocketpp::connection_hdl hdl, client::message_ptr msg) {
	rapidjson::Document doc;
	bool valid_json = asyncflow::util::JsonUtil::ParseJson(msg->get_payload(), doc);
	if (!valid_json)
	{
		REQUIRE(false);
	}
	//std::cout << msg->get_payload() << std::endl;
	auto& map = manager->GetWebsocketManager().GetChartMap();
	if (message_count == 0)
	{
		REQUIRE(strcmp(doc["method"].GetString(), "quick_debug") == 0);
	}
	else if(message_count == 1)
	{
		REQUIRE(strcmp(doc["method"].GetString(), "quick_debug") == 0);
		REQUIRE(doc["params"]["chart_name"] == "test_websocket");
		REQUIRE(doc["params"]["agent_id"].GetInt() == agent->GetId());
	}
	else if (message_count == 2)
	{
		REQUIRE(doc["method"] == "debug_chart");
		REQUIRE(doc["params"]["chart_name"] == "test_websocket");
		auto data = doc["params"]["running_data"].GetArray();
		REQUIRE(data.Size() == 1);
		REQUIRE(data[0]["type"] == "node_status");
		asyncflow::debug::NodeStatusData node_data;
		node_data.Deserialize(data[0]);
		REQUIRE(node_data.old_status == 0);
		REQUIRE(node_data.new_status == 1);
	}
	else if (message_count == 3)
	{
		REQUIRE(doc["method"] == "debug_chart");
		REQUIRE(doc["params"]["chart_name"] == "test_websocket");
		auto data = doc["params"]["running_data"].GetArray();
		REQUIRE(data.Size() == 2);
		REQUIRE(data[0]["type"] == "node_status");
		asyncflow::debug::NodeStatusData node_data;
		node_data.Deserialize(data[0]);
		REQUIRE(node_data.old_status == 0);
		REQUIRE(node_data.new_status == 2);
	}
	else if (message_count == 4)
	{
		REQUIRE(doc["method"] == "stop_chart");
		REQUIRE(doc["params"]["chart_name"] == "test_websocket");
		runflag = false;
	}
	++message_count;
}

TEST_CASE("quick debug test")
{
	INIT_LUA_TEST
	////////////////////////////////////////////////////////////////////////////////////////////	
	auto builder = LuaChartBuilder(L);
	auto* node_0 = builder.AddNode([&]()
	{
		return 0;
	});
	auto* chart_data = builder.Build();
	chart_data->SetName("test_websocket");
	manager->ReloadChartData(chart_data);
	client c;
	std::string url = "ws://localhost:9000";
	c.init_asio();
	c.clear_access_channels(websocketpp::log::alevel::all);
	c.set_message_handler(bind(&quick_debug_on_message, &c, agent, ::_1, ::_2));
	c.set_open_handler(bind(&on_open, &c, quick_debug, ::_1));
	websocketpp::lib::error_code ec;
	auto hdl = c.get_connection(url, ec);
	if (ec)
	{
		ASYNCFLOW_ERR("could not create connection because: {} ", ec.message());
		runflag = false;
	}
	c.connect(hdl);
	runflag = true;
	message_count = 0;
	std::thread t1([&agent]() {run_and_attach(agent); });
	std::thread t2([&c]() {run_web(&c); });
	t1.join();
	t2.join();

	////////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
}

void breakpoint_on_message(client* c, Agent* agent, websocketpp::connection_hdl hdl, client::message_ptr msg) {
	rapidjson::Document doc;
	bool valid_json = asyncflow::util::JsonUtil::ParseJson(msg->get_payload(), doc);
	if (!valid_json)
	{
		REQUIRE(false);
	}
	//std::cout << msg->get_payload() << std::endl;
	auto& map = manager->GetWebsocketManager().GetChartMap();
	if (message_count == 0)
	{
		REQUIRE(strcmp(doc["method"].GetString(), "break_point") == 0);
		try
		{
			c->send(hdl, quick_debug, websocketpp::frame::opcode::text);
		}
		catch (websocketpp::exception const& ec)
		{
			ASYNCFLOW_ERR("send message error in quick_debug because: {0} ", ec.what());
			runflag = false;
		}
	}
	else if (message_count == 1)
	{
		REQUIRE(strcmp(doc["method"].GetString(), "quick_debug") == 0);
		REQUIRE(doc["result"]["chart_name"] == "test_websocket");
	}
	else if (message_count == 2)
	{
		REQUIRE(doc["method"] == "quick_debug");
		REQUIRE(doc["params"]["chart_name"] == "test_websocket");
	}
	else if (message_count == 3)
	{
		REQUIRE(doc["method"] == "debug_chart");
		REQUIRE(doc["params"]["chart_name"] == "test_websocket");
		auto data = doc["params"]["running_data"].GetArray();
		REQUIRE(data.Size() == 1);
		REQUIRE(data[0]["type"] == "node_status");
		asyncflow::debug::NodeStatusData node_data;
		node_data.Deserialize(data[0]);
		REQUIRE(node_data.old_status == 0);
		REQUIRE(node_data.new_status == 1);
	}
	else if (message_count == 4)
	{
		REQUIRE(doc["method"] == "debug_chart");
		REQUIRE(doc["params"]["chart_name"] == "test_websocket");
		auto data = doc["params"]["running_data"].GetArray();
		REQUIRE(data.Size() == 2);
		REQUIRE(data[0]["type"] == "node_status");
		asyncflow::debug::NodeStatusData node_data;
		node_data.Deserialize(data[0]);
		REQUIRE(node_data.old_status == 0);
		REQUIRE(node_data.new_status == 2);
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		try
		{
			c->send(hdl, delete_breakpoint, websocketpp::frame::opcode::text);
		}
		catch (websocketpp::exception const& ec)
		{
			ASYNCFLOW_ERR("send message error in delete_breakpoint because: {0} ", ec.what());
			runflag = false;
		}

	}
	else if (message_count == 5)
	{
		REQUIRE(doc["method"] == "break_point");
		REQUIRE(doc["result"]["command"] == "delete");
	}
	else if (message_count == 6)
	{
		REQUIRE(doc["method"] == "debug_chart");
		REQUIRE(doc["params"]["chart_name"] == "test_websocket");
		auto data = doc["params"]["running_data"].GetArray();
		REQUIRE(data.Size() == 2);
		REQUIRE(data[0]["type"] == "node_status");
		asyncflow::debug::NodeStatusData node_data;
		node_data.Deserialize(data[0]);
		REQUIRE(node_data.old_status == 0);
		REQUIRE(node_data.new_status == 2);
		runflag = false;
	}
	++message_count;
}

TEST_CASE("breakpoint test")
{
	INIT_LUA_TEST
	////////////////////////////////////////////////////////////////////////////////////////////	
	auto* chart_data = get_simple_chartdata(L);
	manager->ReloadChartData(chart_data);
	client c;
	std::string url = "ws://localhost:9000";
	c.init_asio();
	c.clear_access_channels(websocketpp::log::alevel::all);
	c.set_message_handler(bind(&breakpoint_on_message, &c, agent, ::_1, ::_2));
	c.set_open_handler(bind(&on_open, &c, set_breakpoint, ::_1));
	websocketpp::lib::error_code ec;
	auto hdl = c.get_connection(url, ec);
	if (ec)
	{
		ASYNCFLOW_ERR("could not create connection because: {} ", ec.message());
		runflag = false;
	}
	c.connect(hdl);
	runflag = true;
	message_count = 0;
	std::thread t1([&agent]() {run_and_attach(agent); });
	std::thread t2([&c]() {run_web(&c); });
	t1.join();
	t2.join();

	////////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
}

void gm_on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
	rapidjson::Document doc;
	bool valid_json = asyncflow::util::JsonUtil::ParseJson(msg->get_payload(), doc);
	if (!valid_json)
	{
		REQUIRE(false);
	}
	REQUIRE(doc["method"] == "gm");
	REQUIRE(doc["result"][0] == "test_websocket");
	REQUIRE(doc["result"][1] == "111");
	runflag = false;
}

TEST_CASE("gm test")
{
	INIT_LUA_TEST
	////////////////////////////////////////////////////////////////////////////////////////////
	auto* chart_data = get_simple_chartdata(L);
	agent->AttachChart(chart_data);
	client c;
	std::string url = "ws://localhost:9000";
	c.init_asio();
	c.clear_access_channels(websocketpp::log::alevel::all);
	c.set_message_handler(bind(&gm_on_message, &c, ::_1, ::_2));
	c.set_open_handler(bind(&on_open, &c, gm_str, ::_1));
	websocketpp::lib::error_code ec;
	auto hdl = c.get_connection(url, ec);
	if (ec)
	{
		ASYNCFLOW_ERR("could not create connection because: {} ", ec.message());
		runflag = false;
	}
	c.connect(hdl);
	runflag = true;
	std::thread t1([&]() {run_step(); });
	std::thread t2([&c]() {run_web(&c); });
	t1.join();
	t2.join();

	////////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}

#endif