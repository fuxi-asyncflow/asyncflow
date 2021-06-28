#ifdef FLOWCHART_DEBUG
#include<vector>
#include<string>
#include<debug/debugger.h>

namespace asyncflow
{
	namespace debug
	{
		class JsDebugger: public HandleObject
		{
		public:
			void StartDebugChart(core::Chart*) override;
			void StopDebugChart(core::Chart*) override;
			void QuickDebugChart(core::ChartData*) override;
			void ContinueDebugChart(core::Chart*) override;
			void Reply(const std::string& msg) override;

			void SendStopData(core::Chart*);
			void StartQuickDebug(core::Chart*);
			void StopDebugChartWithData(core::Chart*);

			void Step();

		private:
			std::vector<core::Chart*> charts_;
			std::vector<std::string> quick_debug_charts_;
		};
	}
}

#endif