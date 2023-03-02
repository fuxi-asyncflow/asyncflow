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
			DataManager() = default;
			~DataManager()
			{
			    for (auto kv : chart_data_)
			    {
					auto* chart = kv.second;
					delete chart;
			    }
				chart_data_.clear();
			}

			ChartDataMap* GetChartData()
			{				
				return &chart_data_;
			}

		private:
			ChartDataMap chart_data_;
		};
	}
}
