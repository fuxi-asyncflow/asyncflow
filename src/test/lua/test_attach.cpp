#include "export_lua.h"
#include "lua_manager.h"
#include "lua_chart_builder.h"

#include "catch.hpp"

TEST_CASE("test attach")
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
	chart_data->SetName("Test01");
	agent->AttachChart(chart_data);
	dostring(L, "asyncflow.step(0)");
	dostring(L, "asyncflow.step(999)");
	dostring(L, "asyncflow.stop(a,{\"Test01\"})");
	REQUIRE(agent->GetRunningChartNames().size() == 0);
	dostring(L, "asyncflow.step(10)");
	dostring(L, "asyncflow.start(a,{\"Test01\"})");
	dostring(L, "asyncflow.step(0)");
	dostring(L, "asyncflow.step(1000)");
	REQUIRE(wait_node_runned == 1);
	dostring(L, "asyncflow.remove(a,\"Test01\")");
	dostring(L, "asyncflow.step(10)");

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
	delete chart_data;
}

int callback(lua_State* L)
{
	auto str = lua_tostring(L, -1);
	REQUIRE(strcmp(str, "callback") == 0);
	return 0;
}

TEST_CASE("test set callback")
{
	INIT_LUA_TEST;
	///////////////////////////////////////////////////////////////////////////////////////////
	auto builder = LuaChartBuilder(L);
	int wait_node_runned = 0;
	auto* node_0 = builder.AddNode([]()
	{
		lua_pushstring(L, "callback");
		manager->Return(L);
		return 0;
	});
	auto* chart_data = builder.Build();
	chart_data->SetName("Test01");
	manager->ReloadChartData(chart_data);
	dostring(L, "chart = asyncflow.attach(a, 'Test01')");
	lua_register(L, "callback_func", callback);
	dostring(L, "chart:set_callback(callback_func)");
	agent->Start();
	dostring(L, "asyncflow.step(0)");

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
}

TEST_CASE("test set args")
{
	INIT_LUA_TEST;
	///////////////////////////////////////////////////////////////////////////////////////////
	auto builder = LuaChartBuilder(L);
	int wait_node_runned = 0;
	auto* node_0 = builder.AddNode([]()
	{
		return 0;
	});
	auto* chart_data = builder.Build();
	chart_data->SetName("Test01");
	chart_data->SetVarCount(3);
	chart_data->SetParamsCount(3);
	std::vector<Parameter> vars{ Parameter("a", "Number", true), Parameter("b", "Bool", true), Parameter("c", "String", true) };
	chart_data->SetVariables(vars);
	manager->ReloadChartData(chart_data);
	dostring(L, "chart = asyncflow.attach(a, 'Test01',{a = 100, b = true, c = 'hello'})");
	agent->Start();
	lua_getglobal(L, "chart");						
	void* chart_ud = lua_touserdata(L, -1);
	auto* chart = asyncflow::lua::LightUserDataWrapper::Cast<asyncflow::lua::LuaChart*>(chart_ud);
	chart->GetVar(L, 0);
	auto a = lua_tonumber(L, -1);
	REQUIRE(a == 100);
	chart->GetVar(L, 1);
	REQUIRE(lua_isboolean(L, -1));
	REQUIRE(lua_toboolean(L, -1));
	chart->GetVar(L, 2);
	auto* c = lua_tostring(L, -1);
	REQUIRE(strcmp(c, "hello") == 0);
	lua_settop(L, 0);
	lua_pushstring(L, "new_hello");
	chart->SetVar(L, 2);
	chart->GetVar(L, 2);
	REQUIRE(strcmp(lua_tostring(L, -1), "new_hello") == 0);

	///////////////////////////////////////////////////////////////////////////////////////////
	DESTROY_LUA
}
