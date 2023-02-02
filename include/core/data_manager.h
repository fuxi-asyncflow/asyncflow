#pragma once
#include <unordered_map>
#include "core/chart_data.h"

namespace asyncflow
{
	namespace core
	{
		class DataManager
		{
			using ChartDataMap = std::unordered_map<std::string, asyncflow::core::ChartData*>;

		public:
			ChartDataMap chart_data_;

			ChartDataMap* GetChartData()
			{				
				return &chart_data_;
			}
		};
	}
}
