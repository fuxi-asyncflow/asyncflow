#include "export_lua.h"
#include "lua_manager.h"
#include "lua_chart_builder.h"

#include "catch.hpp"
#include <fstream>

const char* chart_json_str = "["
"{"
"	\"name\": \"test_01\","
"		\"path\" : \"AI.test_01\","
"		\"start\" : ["
"			\"da1f1032324e47199d94a4d774cb46c3\","
"		] ,"
"		\"nodes\" : ["
"			{"
"				\"uid\": \"da1f1032324e47199d94a4d774cb46c3\","
"					\"type\" : \"function\","
"					\"funcName\" : \"test_01\","
"					\"success\" : ["
"					] ,"
"					\"fail\" : ["
"					]"
"			}"
"		]"
"} "
"]";

const char* event_json_str = "["
"{ \"id\":1, \"name\" : \"Start\", \"arg_count\" : 0},"
"{ \"id\":2, \"name\" : \"Tick\", \"arg_count\" : 0},"
"{ \"id\":3, \"name\" : \"Event0Arg\", \"arg_count\" : 0},"
"{ \"id\":4, \"name\" : \"Event1Arg\", \"arg_count\" : 1},"
"{ \"id\":5, \"name\" : \"Event2Arg\", \"arg_count\" : 2},"
"]";

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