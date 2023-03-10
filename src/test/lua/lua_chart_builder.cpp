#include "lua_chart_builder.h"

#include "../lua/export_lua.h"

lua_State* L;

void luaL_requiref(lua_State* L, const char* name, lua_CFunction func)
{
	lua_getglobal(L, "package");			// +1
	lua_getfield(L, -1, "preload");			// +1
	lua_pushcfunction(L, func);				// +1
	lua_setfield(L, -2, name);				// -1
	lua_pop(L, 2);							// -2 pop package and preload
}

void init_lua()
{
	L = luaL_newstate();
	luaL_openlibs(L);

	luaL_requiref(L, "asyncflow", luaopen_asyncflow);
}

void destroy_lua()
{
	lua_close(L);
	L = nullptr;
}

int print_lua_error(lua_State* L)
{
	auto* msg = lua_tostring(L, 1);
	printf("%s", msg);
	return 0;
}

void dostring(lua_State* L, const char* str)
{
	lua_pushcfunction(L, print_lua_error);
	auto error_func = lua_gettop(L);
	luaL_loadstring(L, str);
	if (lua_pcall(L, 0, LUA_MULTRET, error_func) != 0)
	{
		printf("\ncall %s error", str);
	}
}


/////////////////////////////////////////////////////////////////


int LuaFuncUserdata::call(lua_State* L)
{
	void* p = lua_touserdata(L, 1);
	auto* self = reinterpret_cast<LuaFuncUserdata*>(p);
	self->_f();
	lua_pushboolean(L, 1);
	return 1;
}

int LuaFuncUserdata::Create(lua_State* L, std::function<void()>&& f)
{
	void* p = lua_newuserdata(L, sizeof(LuaFuncUserdata));			// +1
	new(p)LuaFuncUserdata(std::forward<std::function<void()>&&>(f));

	lua_newtable(L);													// +1
	lua_pushcfunction(L, LuaFuncUserdata::call);						// +1		
	lua_setfield(L, -2, "__call");									// -1
	lua_setmetatable(L, -2);										// -1

	lua_rawgeti(L, LUA_REGISTRYINDEX
		, asyncflow::lua::LuaManager::currentManager->FunctionRef);	//  +1
	lua_pushvalue(L, -2);											//  +1
	auto const r = luaL_ref(L, -2);										//  -1
	lua_pop(L, 2);														//  -2
	return r;
}

asyncflow::lua::LuaChartData* LuaChartBuilder::Build()
{
	auto* chartData = new asyncflow::lua::LuaChartData();
	std::vector<NodeData*> nodes;

	auto* startNode = new NodeData(0);
	nodes.push_back(startNode);
	std::vector<int> orphans;
	for (auto* node : nodes_)
	{
		if (node->is_orphan)
			orphans.push_back(node->id_);
	}
	startNode->SetChildren(orphans, orphans);

	for (auto* node : nodes_)
	{
		auto* nodeData = new NodeData(node->id_);
		auto* nodeFunc = new asyncflow::lua::LuaNodeFunc();
		nodeFunc->SetFunc(node->func_ref_);
		nodeData->SetNodeFunc(nodeFunc);
		nodeData->SetEventNode(node->event_id >= 0);
		nodeData->SetUid(node->uid_);
		nodes.push_back(nodeData);

		std::vector<int> s;
		std::vector<int> f;
		for (auto const& child : node->children)
		{
			switch (child.second)
			{
			case LuaNode::FAIL:
				f.push_back(child.first->id_);
				break;
			case LuaNode::SUCCESS:
				s.push_back(child.first->id_);
				break;
			default:
				f.push_back(child.first->id_);
				s.push_back(child.first->id_);
			}
		}
		nodeData->SetChildren(f, s);
	}

	chartData->SetNodes(nodes);
	return chartData;
}

LuaChartBuilder::~LuaChartBuilder()
{
	for (auto* node : nodes_)
	{
		delete node;
	}
}

void add_node_func(lua_State* L, const char* name, lua_CFunction nodefunc)
{
	CheckLuaStack(0);	
	lua_getglobal(L, "asyncflow");				// +1
	lua_getfield(L, -1, "node_funcs");		// +1
	lua_pushstring(L, name);					// +1
	lua_pushcfunction(L, nodefunc);				// +1
	lua_rawset(L, -3);						// -2	
	lua_pop(L, 2);
}



