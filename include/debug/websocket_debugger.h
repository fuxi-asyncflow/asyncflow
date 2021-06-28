#ifdef FLOWCHART_DEBUG
#include "debug/debugger.h"
#include "debug/websocket_manager.h"
using namespace asyncflow;

namespace asyncflow
{
	namespace debug
	{
		class WebsocketHandleObject : public HandleObject
		{
		public:
			WebsocketHandleObject(WebsocketManager* websocketManager_) :
				websocketManager_(websocketManager_) {};

			void StartDebugChart(core::Chart*) override;
			void StopDebugChart(core::Chart*)  override;
			void QuickDebugChart(core::ChartData*) override;
			void ContinueDebugChart(core::Chart*) override;
			void Reply(const std::string& msg) override;
			void SetHdl(websocketpp::connection_hdl new_hdl) { hdl_ = new_hdl; }
			

		private:
			WebsocketManager* websocketManager_;
			websocketpp::connection_hdl hdl_;
		};
	}
}
#endif