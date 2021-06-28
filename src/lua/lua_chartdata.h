#pragma once

#include "lua_common.h"
#include "core/chart.h"

namespace asyncflow
{
	namespace lua
	{
		class LuaChartData : public core::ChartData
		{
		public:
			~LuaChartData() override;
			core::NodeFunc* CreateNodeFunc(const std::string& code, const std::string& name) override;
		};
	}
}