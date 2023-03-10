#include <fstream>

#include "export_lua.h"
#include "lua_manager.h"
#include "lua_chart_builder.h"

#include "catch.hpp"

TEST_CASE("wait event test")
{
	INIT_LUA_TEST_EVENT
	///////////////////////////////////////////////////////////////////////////////////////////
	auto builder = LuaChartBuilder(L);
	int node_runned = 0;
	auto* node_0 = builder.AddNode([agent]()
		{
			manager->WaitEvent(agent, 3);
			return 0;
		});
	node_0->event_id = 3;

	auto* node_1 = builder.AddNode([&]()
		{
			node_runned = 1;
			return 0;
		});

	node_0->RunBefore(node_1);

	auto* chart_data = builder.Build();
	agent->AttachChart(chart_data);
	agent->Start();
	dostring(L, "asyncflow.step(100)");
	REQUIRE(node_runned == 0);
	manager->Manager::Event(3, agent, nullptr, 0, false);
	REQUIRE(node_runned == 0);
	dostring(L, "asyncflow.step(1)");
	REQUIRE(node_runned == 1);

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}

TEST_CASE("wait other object event test")
{
	INIT_LUA_TEST_EVENT

	dostring(L, "b = {}");
	lua_getglobal(L, "b");
	const void* game_object_b = lua_topointer(L, -1);
	
	dostring(L, "agent_b = asyncflow.register(b, {tick=1000})");
	
	lua_getglobal(L, "agent_b");
	void* ud_b = lua_touserdata(L, -1);
	auto* agent_b = asyncflow::lua::LightUserDataWrapper::Cast<asyncflow::lua::LuaAgent*>(ud_b);
	
	///////////////////////////////////////////////////////////////////////////////////////////
	auto builder = LuaChartBuilder(L);
	int node_runned = 0;
	auto* node_0 = builder.AddNode([agent_b]()
		{
			manager->WaitEvent(agent_b, 3);
			return 0;
		});
	node_0->event_id = 3;

	auto* node_1 = builder.AddNode([&]()
		{
			node_runned = 1;
			return 0;
		});

	node_0->RunBefore(node_1);

	auto* chart_data = builder.Build();
	agent->AttachChart(chart_data);
	agent->Start();
	dostring(L, "asyncflow.step(100)");
	REQUIRE(node_runned == 0);
	manager->Manager::Event(3, agent_b, nullptr, 0, false);
	REQUIRE(node_runned == 0);
	dostring(L, "asyncflow.step(1)");
	REQUIRE(node_runned == 1);

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}

TEST_CASE("test event params")
{
	INIT_LUA_TEST_EVENT
	///////////////////////////////////////////////////////////////////////////////////////////
	auto builder = LuaChartBuilder(L);
	int node_runned = 0;
	auto* node_0 = builder.AddNode([agent]()
		{
			manager->WaitEvent(agent, 4);
			return 0;
		});
	node_0->event_id = 3;

	auto* node_1 = builder.AddNode([&]()
		{
			manager->GetEventParam(L, 4, 0);					
			node_runned = static_cast<int>(lua_tointeger(L, -1));
			lua_pop(L, 1);
			return 0;
		});

	node_0->RunBefore(node_1);

	auto* chart_data = builder.Build();
	agent->AttachChart(chart_data);
	agent->Start();
	dostring(L, "asyncflow.step(100)");
	REQUIRE(node_runned == 0);

	// prepare event arg
	lua_pushinteger(L, 888);
	int arg_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	int* args = new int[1];
	args[0] = arg_ref;
	manager->Manager::Event(4, agent, args, 1, false);
	
	REQUIRE(node_runned == 0);
	dostring(L, "asyncflow.step(1)");
	REQUIRE(node_runned == 888);

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}