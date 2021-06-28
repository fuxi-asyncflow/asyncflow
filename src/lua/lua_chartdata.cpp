#include "lua_chartdata.h"
#include "lua_nodefunc.h"
#include "lua_manager.h"

using namespace asyncflow::lua;
///////////////////////////////////////////////////////////////////////////
LuaChartData::~LuaChartData()
{
}

NodeFunc* LuaChartData::CreateNodeFunc(const std::string& code, const std::string& name)
{
	return LuaNodeFunc::GetFuncFromString(code, name);
}