#include "export_lua.h"
#include "lua_manager.h"
#include "lua_chart_builder.h"

#include "catch.hpp"
#include <vector>

TEST_CASE("subchart test")
{
	INIT_LUA_TEST;
	///////////////////////////////////////////////////////////////////////////////////////////
	auto subchart_builder = LuaChartBuilder(L);
	const int return_value = 888;
	int subchart_runned = 0;
	std::string sub_chart_name = "AI.subchart";
	auto* node_0 = subchart_builder.AddNode([&]()
		{
			subchart_runned = 1;
			return 0;
		});

	auto* node_1 = subchart_builder.AddNode([&]()
		{
			lua_pushinteger(L, return_value);
			manager->Return(L);
			return 0;
		});	

	node_0->RunBefore(node_1);
	auto* sub_chart_data = subchart_builder.Build();
	sub_chart_data->SetName(sub_chart_name);
	manager->ReloadChartData(sub_chart_data);

	int main_node_runned = 0;
	auto builder = LuaChartBuilder(L);
	auto* node_main_0 = builder.AddNode([&]()
		{
			lua_settop(L, 0);
			lua_getglobal(L, "a");
			auto* pointer = lua_topointer(L, 1);
			manager->Subchart(sub_chart_name, (void*)pointer,L, 0);
			return 0;
		});	

	auto* node_main_1 = builder.AddNode([&]()
		{
			main_node_runned = 1;
			return 0;
		});
	node_main_0->RunBefore(node_main_1);
	auto* chart_data = builder.Build();
	
	agent->AttachChart(chart_data);
	agent->Start();
	dostring(L, "asyncflow.step(0)");
	if(manager->IsImmediateSub())
	{
		REQUIRE(subchart_runned == 1);
		REQUIRE(main_node_runned == 1);
	}
	else
	    REQUIRE(subchart_runned == 0);
	dostring(L, "asyncflow.step(1)");
	REQUIRE(subchart_runned == 1);
	if(!manager->IsImmediateSub())
	    REQUIRE(main_node_runned == 0);
	dostring(L, "asyncflow.step(1)");	
	REQUIRE(main_node_runned == 1);
	

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}