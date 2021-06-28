
#include "export_lua.h"
#include "lua_manager.h"
#include "lua_chart_builder.h"


#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("manager setup")
{
	init_lua();
	auto name_methed = Agent::DEBUG_NAME_METHOD;
	dostring(L, "cfg = {loop_check = false, immediate_subchart = true, default_timestep = 10, debug_ip = \"0.0.0.0\", debug_port = 8080, debug_name_function =\"get_name\"}");
	dostring(L, "require('asyncflow')");
	dostring(L, "asyncflow.setup(cfg)");
	manager = (asyncflow::lua::LuaManager*)get_manager();
	REQUIRE(manager != nullptr);
	REQUIRE(manager->IsImmediateSub() == true);
#ifdef FLOWCHART_DEBUG
	REQUIRE(Agent::DEBUG_NAME_METHOD == "get_name");
	Agent::DEBUG_NAME_METHOD = name_methed;
#endif
	DESTROY_LUA
}

TEST_CASE("agent lifetime")
{
	init_lua();
	dostring(L, "require('asyncflow')");
	dostring(L, "asyncflow.setup()");
	manager = (asyncflow::lua::LuaManager*)get_manager();
	REQUIRE(manager != nullptr);

	dostring(L, "a = { GetAsyncFlowName = function(self) return 'a' end}");		
	lua_getglobal(L, "a");
	const void* game_object = lua_topointer(L, -1);	

	dostring(L, "agent = asyncflow.register(a, {tick=1000})");

	lua_getglobal(L, "agent");
	void* ud = lua_touserdata(L, -1);
	auto* agent = asyncflow::lua::LightUserDataWrapper::Cast<asyncflow::lua::LuaAgent*>(ud);
	REQUIRE(agent->GetName() == "a");

	REQUIRE(agent->GetGameObject() == game_object);
	auto const agent_id = agent->GetId();
	REQUIRE(manager->GetAgentManager().GetAgentById(agent_id) == agent);

	lua_rawgeti(L, LUA_REGISTRYINDEX, manager->ObjectRef);
	lua_rawgeti(L, -1, agent->GetRefObject());
	REQUIRE(lua_topointer(L, -1) == game_object);
	lua_pop(L, 2);

	// deregister_obj agent
	dostring(L, "asyncflow.step(100)");
	dostring(L, "asyncflow.deregister(a)");	
	
	REQUIRE(agent->GetStatus() == Agent::Destroying);
	REQUIRE(manager->GetAgentManager().GetAgentById(agent->GetId()) == agent);	

	// after one step, the agent should deregister_obj and removed from agent manager
	dostring(L, "asyncflow.step(100)");
	REQUIRE(manager->GetAgentManager().GetAgentById(agent_id) == nullptr);

	DESTROY_LUA
}

TEST_CASE("agent not deregister before manager destructor")
{	
	// check program not crash if agent not deregister before manager destructor, lua agent destructor called after lua manager destructor will crash
	INIT_LUA_TEST;
	DESTROY_LUA;
}

TEST_CASE("dfs test")
{
	INIT_LUA_TEST

	////////////////////////////////////////////////////////////////////////////////////////////	
	auto builder = LuaChartBuilder(L);
	int count = 0;
	auto* node_0 = builder.AddNode([&count]()
	{		
		REQUIRE(count == 0);
		count++;
		return 0;				
	});
	
	auto* node_1 = builder.AddNode([&count]()
	{
		REQUIRE(count == 1);
		count++;		
		return 0;
	});

	auto* node_2 = builder.AddNode([&count]()
	{
		REQUIRE(count == 2);
		count++;
		return 0;
	});

	auto* node_3 = builder.AddNode([&count]()
	{
		REQUIRE(count == 3);
		count++;
		return 0;
	});

	/*                    0
	 *                  /   \
	 *                1      3
	 *               /
	 *              2 
	 */               
	
	node_0->RunBefore(node_1);
	node_0->RunBefore(node_3);
	node_1->RunBefore(node_2);

	////////////////////////////////////////////////////////////////////////////////////////////
	
	auto* chart_data = builder.Build();
	agent->AttachChart(chart_data);
	agent->Start();
	dostring(L, "asyncflow.step(100)");
	dostring(L, "asyncflow.step(100)");

	DESTROY_LUA
	delete chart_data;
}

TEST_CASE("loop test")
{
	INIT_LUA_TEST

	////////////////////////////////////////////////////////////////////////////////////////////	
	auto builder = LuaChartBuilder(L);
	int count = 0;
	auto* node_0 = builder.AddNode([&count]()
		{
			REQUIRE(count == 0);
			count++;
			return 0;
		});

	auto* node_1 = builder.AddNode([&count]()
		{			
			count++;
			return 0;
		});

	auto* node_2 = builder.AddNode([&count]()
		{			
			count+=10;			
			return 0;
		});



	/*                    0
	 *                  /   
	 *                1      
	 *               /  \
	 *              2 - -
	 */

	node_0->RunBefore(node_1);
	node_1->RunBefore(node_2);
	node_2->RunBefore(node_1);

	////////////////////////////////////////////////////////////////////////////////////////////

	auto* chart_data = builder.Build();
	agent->AttachChart(chart_data);
	agent->Start();
	dostring(L, "asyncflow.step(100)");
	REQUIRE(count == 12);
	dostring(L, "asyncflow.step(800)");
	REQUIRE(count == 12);
	dostring(L, "asyncflow.step(100)");	
	REQUIRE(count == 23);

	DESTROY_LUA
	delete chart_data;
}

TEST_CASE("node run error test")
{
	INIT_LUA_TEST

	////////////////////////////////////////////////////////////////////////////////////////////	
	auto builder = LuaChartBuilder(L);
	int count = 0;
	auto* node_0 = builder.AddNode([&count]()
		{
			REQUIRE(count == 0);
			count++;
			return 0;
		});

	auto* node_1 = builder.AddNode([&count]()
		{
			luaL_error(L, "node run error test");
			count++;
			return 0;
		});	

	node_0->RunBefore(node_1);	

	////////////////////////////////////////////////////////////////////////////////////////////

	auto* chart_data = builder.Build();
	agent->AttachChart(chart_data);
	agent->Start();
	dostring(L, "asyncflow.step(100)");	
	
	REQUIRE(count == 1);

	DESTROY_LUA
	delete chart_data;
}
