#pragma once
#include "lua_manager.h"
#include "lua_common.h"
#include <functional>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

extern lua_State* L;
static asyncflow::lua::LuaManager* manager = nullptr;

void luaL_requiref(lua_State* L, const char* name, lua_CFunction func);

void init_lua();

void destroy_lua();

int print_lua_error(lua_State* L);

void dostring(lua_State* L, const char* str);

class LuaFuncUserdata
{
public:
	LuaFuncUserdata(std::function<void()>&& f)
		: _f(f)
	{	}

	static int call(lua_State* L);
	static int Create(lua_State* L, std::function<void()>&& f);
	
private:
	std::function<void()> _f;
};

class LuaNode
{
public:
	enum Condition
	{
		FAIL = 0,
		SUCCESS = 1,
		ALWAYS = 2
	};

	LuaNode(int func_ref, int id)
		: id_(id), uid_(""), func_ref_(func_ref), is_orphan(true), event_id(-1)
	{

	}
	void RunBefore(LuaNode* node, Condition cond = ALWAYS)
	{
		children.push_back(std::pair<LuaNode*, Condition>(node, cond));
		node->is_orphan = false;
	}

	void RunAfter(LuaNode* node, Condition cond = ALWAYS)
	{
		node->RunBefore(this, cond);
	}

	int Id() const { return id_; }

public:
	int id_;
	std::string uid_;
	int event_id;
	int func_ref_;
	bool is_orphan;
	std::vector<std::pair<LuaNode*, Condition>> children;
};

class LuaChartBuilder
{
public:
	LuaChartBuilder(lua_State* L)
		: L_(L)
	{

	}
	~LuaChartBuilder();
	LuaNode* AddNode(std::function<void()>&& f)
	{
		auto const func_ref = LuaFuncUserdata::Create(L_, std::forward<std::function<void()>&&>(f));
		auto* node = new LuaNode(func_ref, (int)nodes_.size() + 1);
		nodes_.push_back(node);
		return node;
	}
	asyncflow::lua::LuaChartData* Build();	

private:
	std::list<LuaNode*> nodes_;
	lua_State* L_;
};

///////////////////////////////////////////////////////////////////////////////////////

#define INIT_LUA_TEST \
init_lua();		\
dostring(L, "require('asyncflow')");		\
											\
dostring(L, "asyncflow.setup()");			\
manager = (asyncflow::lua::LuaManager*)get_manager();	\
REQUIRE(manager != nullptr);							\
														\
dostring(L, "a = {}");									\
lua_getglobal(L, "a");									\
const void* game_object = lua_topointer(L, -1);			\
														\
dostring(L, "agent = asyncflow.register(a, {tick=1000})");	\
														\
lua_getglobal(L, "agent");								\
void* ud = lua_touserdata(L, -1);						\
auto* agent = asyncflow::lua::LightUserDataWrapper::Cast<asyncflow::lua::LuaAgent*>(ud);


#define DESTROY_LUA \
	delete manager;	\
	destroy_lua();

///////////////////////////////////////////////////////////////////////////////////////

static const char* event_info = "[\n"
"{\"id\":1, \"name\" : \"Start\", \"arg_count\" : 0},\n"
"{ \"id\":2, \"name\" : \"Tick\", \"arg_count\" : 0 },\n"
"{ \"id\":3, \"name\" : \"Event0Arg\", \"arg_count\" : 0 },\n"
"{ \"id\":4, \"name\" : \"Event1Arg\", \"arg_count\" : 1 },\n"
"{ \"id\":5, \"name\" : \"Event2Arg\", \"arg_count\" : 2 },\n"
"]";

#define INIT_LUA_TEST_EVENT \
init_lua();		\
dostring(L, "require('asyncflow')");		\
											\
dostring(L, "asyncflow.setup()");			\
manager = (asyncflow::lua::LuaManager*)get_manager();	\
\
std::ofstream file("tmp.txt");		\
file << event_info;					\
file.close();						\
manager->ImportEvent("tmp.txt");	\
\
REQUIRE(manager != nullptr);							\
														\
dostring(L, "a = {}");									\
lua_getglobal(L, "a");									\
const void* game_object = lua_topointer(L, -1);			\
														\
dostring(L, "agent = asyncflow.register(a, {tick=1000})");	\
														\
lua_getglobal(L, "agent");								\
void* ud = lua_touserdata(L, -1);						\
auto* agent = asyncflow::lua::LightUserDataWrapper::Cast<asyncflow::lua::LuaAgent*>(ud);


void add_node_func(lua_State* L, const char* name, lua_CFunction nodefunc);