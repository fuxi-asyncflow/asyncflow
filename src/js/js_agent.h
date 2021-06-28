#pragma once
#include "core/agent.h"

namespace asyncflow
{
	namespace js
	{
		class JsAgent : public core::Agent
		{
		public:
			typedef int TOBJ;
			JsAgent(Manager* manager, int id);
			virtual ~JsAgent();
			int	GetRefObject() { return obj_; }
			TOBJ GetGameObject() { return obj_; };

		private:
			int obj_;
		};
	}
}