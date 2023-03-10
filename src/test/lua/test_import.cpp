#include "export_lua.h"
#include "lua_manager.h"
#include "lua_chart_builder.h"

#include "catch.hpp"
#include <fstream>

const char* chart_json_str = "--- \n"
"path: AI.test_01\n"
"uid : 3f73e8cd-591a-4bc7-ae02-00b58135f99f\n"
"type : Character\n"
"nodes :\n"
"- \n"
"  uid : a7f464c4-7867-468e-921d-d5a2fad1eba5\n"
"- \n"
"  uid : 9dfc8ea5-8a47-43db-9518-3f7d9bfb04ec\n"
"  text : \"Say(\\\"hello\\\")\"\n"
"  code :\n"
"    type : FUNC\n"
"    func_name: test_01\n"
"connectors :\n"
"- \n"
"  start : a7f464c4-7867-468e-921d-d5a2fad1eba5\n"
"  end : 9dfc8ea5-8a47-43db-9518-3f7d9bfb04ec\n"
"  type : 2\n"
"...";

const char* event_json_str = "--- \n"
"- \n"
"  id: 1\n"
"  name : Start\n"
"- \n"
"  id: 2\n"
"  name : Tick\n"
"- \n"
"  id: 3\n"
"  name : Event0Arg\n"
"- \n"
"  id: 4\n"
"  name : Event1Arg\n"
"  parameters: \n"
"  - {name: param, type : String}\n"
"- \n"
"  id : 5\n"
"  name : Event2Arg\n"
"  parameters: \n"
"  - {name: param1, type : String}\n"
"  - {name: param2, type : String}\n"
"...";

int flag = 0;
int test_01(lua_State* L)
{
	flag = 1;	
	return 0;
}

TEST_CASE("import event test")
{
	INIT_LUA_TEST;
	std::ofstream file("tmp.txt");
	file << event_json_str;
	file.close();
	int count = manager->ImportEvent("tmp.txt");
	REQUIRE(count == 5);
	DESTROY_LUA
}

TEST_CASE("import chart test")
{
	INIT_LUA_TEST;
	///////////////////////////////////////////////////////////////////////////////////////////
	add_node_func(L, "test_01", test_01);
	std::ofstream file("tmp.txt");
	file << chart_json_str;
	file.close();
	int count = manager->ImportFile("tmp.txt");
	REQUIRE(count == 1);

	auto* chart = manager->AttachChart(const_cast<void*>(game_object), "AI.test_01");
	agent->Start();
	REQUIRE(chart != nullptr);

	// check chart data node count is correct
	auto* chart_data = chart->GetData();
	REQUIRE(chart_data->GetNodeCount() == 2);

	// check test_01 is called
	REQUIRE(flag == 0);
	dostring(L, "asyncflow.step(1)");
	REQUIRE(flag == 1);

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
}

const char* error_json_str = "[" "\n"
"{" "\n"
"	\"name\": \"test_01\"," "\n"
"		\"path\" : \"AI.test_01\"," "\n"
"		\"start\" : " "\n"
"			\"da1f1032324e47199d94a4d774cb46c3\"," "\n"
"		] ," "\n"
"		\"nodes\" : [" "\n"
"			{" "\n"
"				\"uid\": \"da1f1032324e47199d94a4d774cb46c3\"," "\n"
"					\"type\" : \"function\"," "\n"
"					\"funcName\" : \"test_01\"," "\n"
"					\"success\" : [" "\n"
"					] ," "\n"
"					\"fail\" : [" "\n"
"					]" "\n"
"			}" "\n"
"		]" "\n"
"} " "\n"
"]";

TEST_CASE("import json error")
{
	INIT_LUA_TEST;	
	std::ofstream file("tmp.txt");
	file << error_json_str;
	file.close();
	int count = manager->ImportFile("tmp.txt");
	REQUIRE(count == 0);
	DESTROY_LUA
}