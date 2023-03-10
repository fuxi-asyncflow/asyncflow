#include "export_lua.h"
#include "lua_manager.h"
#include "lua_chart_builder.h"

#include "catch.hpp"

TEST_CASE("wait node test")
{
	INIT_LUA_TEST;
	///////////////////////////////////////////////////////////////////////////////////////////
	auto builder = LuaChartBuilder(L);
	int wait_node_runned = 0;
	auto* node_0 = builder.AddNode([]()
		{
			manager->Wait(1000);
			return 0;
		});

	auto* node_1 = builder.AddNode([&]()
		{
			wait_node_runned = 1;
			return 0;
		});

	node_0->RunBefore(node_1);

	auto* chart_data = builder.Build();
	agent->AttachChart(chart_data);
	agent->Start();
	dostring(L, "asyncflow.step(0)");
	dostring(L, "asyncflow.step(999)");
	REQUIRE(wait_node_runned == 0);
	dostring(L, "asyncflow.step(1)");	
	REQUIRE(wait_node_runned == 1);

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}

TEST_CASE("wait node test - time changed")
{
	INIT_LUA_TEST;
	///////////////////////////////////////////////////////////////////////////////////////////
	const int FIRST_WAIT_TIME = 1000;
	const int SECOND_WAIT_TIME = 500;
	auto builder = LuaChartBuilder(L);
	int wait_node_runned = 0;
	int wait_time = 1000;

	auto* node_0 = builder.AddNode([&wait_time]() {});

	auto* node_1 = builder.AddNode([&wait_time]()
		{
			manager->Wait(wait_time);
			return 0;
		});

	auto* node_2 = builder.AddNode([&]()
		{
			wait_node_runned++;
			wait_time = 500;
			return 0;
		});

	node_0->RunBefore(node_1);
	node_1->RunBefore(node_2);
	node_2->RunBefore(node_1);

	auto* chart_data = builder.Build();
	agent->AttachChart(chart_data);
	agent->Start();
	dostring(L, "asyncflow.step(0)");
	dostring(L, fmt::format("asyncflow.step({})", FIRST_WAIT_TIME - 1).c_str());
	REQUIRE(wait_node_runned == 0);
	dostring(L, "asyncflow.step(1)");
	REQUIRE(wait_node_runned == 1);
	dostring(L, fmt::format("asyncflow.step({})", SECOND_WAIT_TIME - 1).c_str());
	REQUIRE(wait_node_runned == 1);
	dostring(L, "asyncflow.step(1)");
	REQUIRE(wait_node_runned == 2);

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}