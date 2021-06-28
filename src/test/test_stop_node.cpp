#include "export_lua.h"
#include "lua_manager.h"
#include "lua_chart_builder.h"

#include "catch.hpp"
#include <vector>

TEST_CASE("stop node test")
{
	INIT_LUA_TEST;
	///////////////////////////////////////////////////////////////////////////////////////////
	auto builder = LuaChartBuilder(L);
	int node_1_runned = 0;
	int node_3_runned = 0;
	auto* node_0 = builder.AddNode([]()
		{
			manager->Wait(1000);
			return 0;
		});

	auto* node_1 = builder.AddNode([&]()
		{
			node_1_runned = 1;
			return 0;
		});

	auto* node_2 = builder.AddNode([&]()
		{
			manager->Wait(1000);
			return 0;
		});

	auto* node_3 = builder.AddNode([&]()
		{
			node_3_runned = 1;
			return 0;
		});

	auto* node_4 = builder.AddNode([&]()
		{
			manager->Wait(500);
			return 0;
		});

	auto* node_5 = builder.AddNode([&]()
		{
			manager->StopNode(std::vector<int>{node_0->Id()});
			return 0;
		});	

	node_0->RunBefore(node_1);
	node_2->RunBefore(node_3);
	node_4->RunBefore(node_5);

	auto* chart_data = builder.Build();
	agent->AttachChart(chart_data);
	agent->Start();
	dostring(L, "asyncflow.step(0)");
	dostring(L, "asyncflow.step(999)");
	REQUIRE(node_1_runned == 0);
	REQUIRE(node_3_runned == 0);
	dostring(L, "asyncflow.step(1)");	
	REQUIRE(node_1_runned == 0);
	REQUIRE(node_3_runned == 1);

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}

TEST_CASE("stop flow test")
{
	INIT_LUA_TEST;
	///////////////////////////////////////////////////////////////////////////////////////////
	auto builder = LuaChartBuilder(L);
	int node_1_runned = 0;
	int node_3_runned = 0;
	auto* node_start = builder.AddNode([]()
		{			
			return 0;
		});
	
	auto* node_0 = builder.AddNode([]()
		{
			manager->Wait(1000);
			return 0;
		});

	auto* node_1 = builder.AddNode([&]()
		{
			node_1_runned = 1;
			return 0;
		});

	auto* node_2 = builder.AddNode([&]()
		{
			manager->Wait(1000);
			return 0;
		});

	auto* node_3 = builder.AddNode([&]()
		{
			node_3_runned = 1;
			return 0;
		});

	auto* node_4 = builder.AddNode([&]()
		{
			manager->Wait(500);
			return 0;
		});

	auto* node_5 = builder.AddNode([&]()
		{
			manager->StopFlow(std::vector<int>{node_start->Id()});
			return 0;
		});

	node_start->RunBefore(node_0);
	node_start->RunBefore(node_2);
	node_start->RunBefore(node_4);
	node_0->RunBefore(node_1);
	node_2->RunBefore(node_3);
	node_4->RunBefore(node_5);

	auto* chart_data = builder.Build();
	agent->AttachChart(chart_data);
	agent->Start();
	dostring(L, "asyncflow.step(0)");
	dostring(L, "asyncflow.step(999)");
	REQUIRE(node_1_runned == 0);
	REQUIRE(node_3_runned == 0);
	dostring(L, "asyncflow.step(1)");
	REQUIRE(node_1_runned == 0);
	REQUIRE(node_3_runned == 0);

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
		delete chart_data;
}