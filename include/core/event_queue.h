#pragma once
#include <deque>

#include "async_event.h"

namespace asyncflow
{
	namespace core
	{
		class EventQueue
		{
			using Event = AsyncEventBase;
			
		public:
			EventQueue() = default;			
			~EventQueue()
			{
				for (auto* ev : in_event_list_)
					delete ev;
				for (auto* ev : out_event_list_)
					delete ev;
			}
			
			void	Switch() { in_event_list_.swap(out_event_list_); }
			
			void	AddEvent(Event* ev, bool immediate = false) { immediate ? out_event_list_.push_front(ev) : in_event_list_.push_back(ev); }
			
			Event*	NextEvent()
			{
				if (out_event_list_.empty())
					return nullptr;
				auto* ev = out_event_list_.front();
				out_event_list_.pop_front();
				return ev;
			}
		private:			
			std::deque<Event*> in_event_list_;		// handle events
			std::deque<Event*> out_event_list_;	// preparing to handle events
		};
	}
}
