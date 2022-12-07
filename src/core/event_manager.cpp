#include "core/event_manager.h"
#include "util/log.h"
#include "util/json.h"
#include "rapidyaml.hpp"

using namespace asyncflow::core;

int AsyncEventBase::START_EVENT = 1;
int AsyncEventBase::TICK_EVENT = 2;

EventManager::EventManager()
{
	event_info_list_.emplace_back(EventInfo{ 0, 0, std::string("None") });
	event_info_list_.emplace_back(EventInfo{ 1, 0, std::string("Start") });
	event_info_list_.emplace_back(EventInfo{ 2, 0, std::string("Tick") });
}

const std::string& EventManager::GetEventName(int event_id) const
{
	if (event_id >= event_info_list_.size())
	{
		ASYNCFLOW_WARN("event id {0} is out of range", event_id);
		event_id = 0;
	}
	return event_info_list_[event_id].name;
}

int EventManager::InitFromJson(const std::string& json_str)
{
	rapidjson::Document doc;
	auto const parse_result = asyncflow::util::JsonUtil::ParseJson(json_str, doc);
	if (!parse_result)
	{
		ASYNCFLOW_ERR("load event info error");
		return 0;
	}

	event_info_list_.clear();
	event_info_list_.emplace_back(EventInfo{ 0, 0, std::string("None") });
	auto event_array = doc.GetArray();
	int count = 0;
	for (auto* it = event_array.begin(); it != event_array.end(); ++it)
	{
		auto id = it->FindMember("id")->value.GetInt();
		auto const name = it->FindMember("name")->value.GetString();
		if (strcmp(name, "Start") == 0)
		{
			AsyncEventBase::START_EVENT = id;
			ASYNCFLOW_DBG("Start event id is {0}", id);
		}
		else if (strcmp(name, "Tick") == 0)
		{
			AsyncEventBase::TICK_EVENT = id;
			ASYNCFLOW_DBG("Tick event id is {0}", id);
		}
		int argc = 0;
		auto it_argc = it->FindMember("arg_count");
		if (it_argc != it->MemberEnd())
		{
			argc = it_argc->value.GetInt();
		}
		ASYNCFLOW_DBG("load event [{0}] {1} {2}", id, argc, id);
		event_info_list_.emplace_back(id, argc, std::string(name));
		++count;
	}
	return count;
}

int EventManager::InitFromYaml(const std::string& yaml_str)
{  
    ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(yaml_str));
    const ryml::NodeRef root = tree.rootref();
    if (!root.is_stream())
        return -1;
    for(const auto& doc : root.children())
    {
        event_info_list_.clear();
        event_info_list_.emplace_back(EventInfo{ 0, 0, std::string("None") });
        for(const auto& ev_node : doc.children())
        {
            auto id_val = ev_node["id"].val();
            int id = std::stoi(std::string(id_val.data(), id_val.size()));
            auto name_val = ev_node["name"].val();
            auto name = std::string(name_val.data(), name_val.size());
            auto params_node = ev_node.find_child("parameters");
            auto argc = params_node.valid() ? params_node.num_children() : 0;
            ASYNCFLOW_DBG("load event [{0}] {1} {2}", id, argc, id);
            event_info_list_.emplace_back(id, static_cast<int>(argc), name);
        }        
    }
    return static_cast<int>(event_info_list_.size()) - 1;
}