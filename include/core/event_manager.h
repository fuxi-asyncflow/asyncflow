#pragma once
#include <vector>
#include <string>
#include "async_event.h"

namespace asyncflow
{
	namespace core
	{
		struct EventInfo
		{
			EventInfo(int i, int a, std::string n)
				: id(i), arg_count(a), name(n) {}
			int id;
			int arg_count;
			std::string name;
		};

		class EventManager
		{
		public:
			EventManager();

			int InitFromJson(const std::string& json_str);
            int InitFromYaml(const std::string& yaml_str);
		
			int GetEventArgsCount(int event_id)
			{
				if(event_id < event_info_list_.size())
				{
					return event_info_list_[event_id].arg_count;
				}
				return -1;				
			}

			int GetEventCount()
			{
				return static_cast<int>(event_info_list_.size());
			}

			AsyncEventBase* GetEvent(int event_id)
			{
				return event_list_[event_id];
			}

			const std::string& GetEventName(int event_id) const;

			std::vector<EventInfo>& GetEventList() { return event_info_list_; }


		private:
			std::vector<EventInfo> event_info_list_;
			std::vector<AsyncEventBase*> event_list_;
		};
	}
}
