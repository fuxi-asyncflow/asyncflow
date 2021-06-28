#pragma once
#include "core/chart.h"
#include<vector>

namespace asyncflow
{
	namespace js
	{
		class JsChart : public core::Chart
		{
		public:
			JsChart() : callback_(false) {};
			~JsChart();
			void SetArgs(void* args, int argc) override;
			void ClearVariables() override;
			void ResetVariables() override;
			bool InitArgs() override;

			void SetVar(int idx, int id);
			int GetVar(int idx);
			void Return(int idx);
			void Return(bool result) override;
			void SetCall() { callback_ = true; }

#ifdef FLOWCHART_DEBUG
			void SendEventStatus(std::string node_uid, const AsyncEventBase* event) override;
#endif

		private:
			std::vector<int> variables_;
			bool callback_;
		};
	}
}