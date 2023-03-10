#include "export_lua.h"
#include "lua_manager.h"
#include "lua_chart_builder.h"

#include "catch.hpp"

TEST_CASE("wait all test")
{
	INIT_LUA_TEST;
	///////////////////////////////////////////////////////////////////////////////////////////
	auto builder = LuaChartBuilder(L);
	int wait_node_runned = 0;
	auto* node_0 = builder.AddNode([&]()
		{
			REQUIRE(wait_node_runned == 0);
			return 0;
		});

	auto* node_1 = builder.AddNode([&]()
		{
			REQUIRE(wait_node_runned == 0);
			return 0;
		});

	auto* node_2 = builder.AddNode([&]()
		{
			REQUIRE(wait_node_runned == 0);
			return 0;
		});

	auto* node_3 = builder.AddNode([&]()
		{
			manager->WaitAll(std::vector<int> {1, 2, 3});
			return 0;
		});
	node_3->event_id = 1;

	auto* node_4 = builder.AddNode([&]()
		{
			wait_node_runned += 1;
			return 0;
		});

	//          start
	//      /     |    \
	//     0      1      2
	//      \     |    /
	//            3
	//            |
	//            4
	node_0->RunBefore(node_3);
	node_1->RunBefore(node_3);
	node_2->RunBefore(node_3);
	node_3->RunBefore(node_4);

	auto* chart_data = builder.Build();	
	agent->AttachChart(chart_data);
	agent->Start();
	dostring(L, "asyncflow.step(50)");	
	REQUIRE(wait_node_runned == 1);

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}