#include "core/timer_manager.h"
#include "core/async_event.h"
#include "core/manager.h"
#include "util/log.h"

using namespace asyncflow::core;

AgentTimer::AgentTimer(Agent* agent)
	: agent_(agent)
{
}

AgentTimer::~AgentTimer()
{
	Stop();
}

void AgentTimer::Start(int delay)
{	
	agent_->GetManager()->GetTimerManager().AddTimer(this, static_cast<int64_t>(agent_->GetTickInterval()) + delay);
}

void AgentTimer::execute()
{
	ASYNCFLOW_DBG("agent tick {0}", agent_->GetManager()->Now());
	if (agent_->IsRunning())
	{
		//tick event
		agent_->GetManager()->Event(AsyncEventBase::TICK_EVENT, agent_, nullptr, 0);
	}
	Start();
}

/////////////////////////////////////////////////////////////////////////////////////////

void TimerManager::AddTimer(TimerEventInterface* timer, uint64_t tick)
{
	timer_wheel_.schedule(timer, tick);
}

void TimerManager::Step(int ms)
{
	if(ms > 0)
		timer_wheel_.advance(ms);
}
