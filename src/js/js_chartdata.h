#pragma once
#include"core/chart.h"

namespace asyncflow
{
	namespace js
	{
		class JsChartData : public core::ChartData
		{
		public:
			core::NodeFunc* CreateNodeFunc(const std::string& code, const std::string& name) override;
		};
	}
}

