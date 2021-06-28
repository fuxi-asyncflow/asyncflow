#pragma once
//#include "photon/Timer.hpp"
#include "ratas.h"
#include <functional>

namespace asyncflow
{
	namespace core
	{
		class Agent;
		class ITick
		{
		public:			
			virtual int operator()() = 0;
		};

		class AgentTimer : public TimerEventInterface
		{
		public:
			AgentTimer(Agent* agent);
			AgentTimer(const AgentTimer& at) = delete;
			~AgentTimer();
			void Start(int delay = 0);
			void Stop() { cancel(); }
			
			
		private:
			Agent* agent_;
			void execute() override;
			
		};

		class TimerManager
		{
		public:
			int64_t Now() { return timer_wheel_.now(); }
			void AddTimer(TimerEventInterface* timer, uint64_t tick);
			void Step(int ms);
		private:
			int64_t now_;
			TimerWheel timer_wheel_;
			
		};
	}
}