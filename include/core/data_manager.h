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
			static ChartDataMap* chart_data_;

			ChartDataMap* GetPublicChartData()
			{
				if (chart_data_ == nullptr)
					chart_data_ = new ChartDataMap();
				return chart_data_;
			}

			ChartDataMap* GetChartData()
			{
				auto chart_data = new ChartDataMap();
				return chart_data;
			}

			bool ClearChartDataMap(ChartDataMap* map_ptr)
			{
				if (map_ptr == nullptr || map_ptr == chart_data_)
					return true;

				for (auto kv : *map_ptr)
				{
					delete kv.second;
				}
				map_ptr->clear();
				delete map_ptr;
				return true;
			}

		};
	}
}
