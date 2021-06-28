#pragma once

#include "core/chart.h"
#include <Python.h>

namespace asyncflow
{
	namespace py
	{
		class PyChartData : public core::ChartData
		{
		public:
			~PyChartData() override;
			core::NodeFunc* CreateNodeFunc(const std::string& code, const std::string& name) override;
		};
	}
}