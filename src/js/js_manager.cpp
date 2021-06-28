#include "js_manager.h"

using namespace asyncflow::js;
using namespace asyncflow::core;

Agent* JsManager::RegisterGameObject(int obj, int tick)
{
	if (tick <= 0)
		tick = DEFAULT_AGENT_TICK;

	if (agent_manager_.GetAgent(obj) != nullptr)
	{
		ASYNCFLOW_LOG("object has registered to asyncflow");
		return nullptr;
	}
	ASYNCFLOW_LOG("register object to asyncflow");
	auto agent = agent_manager_.Register(obj);
	agent->SetTickInterval(tick);
	return agent;
}

Chart* JsManager::AttachChart(int obj, const std::string& chart_name)
{
	auto* agent = agent_manager_.GetAgent(obj);
	if (agent == nullptr)
	{
		ASYNCFLOW_WARN("object is not registered to asyncflow");
		return nullptr;
	}
	return Manager::AttachChart(agent, chart_name);
}

bool JsManager::Event(int event_id, int obj_id, int* args, int arg_count, bool immediate)
{
	return Manager::Event(event_id, agent_manager_.GetAgent(obj_id), (void*)args, arg_count, immediate);
}

int JsManager::GetEventPatam(int event_id, int param_idx)
{
	if (event_id != current_event_->Id())
	{
		ASYNCFLOW_WARN("event id unmatch when get event param");
	}
	auto id = ((AsyncEvent*)current_event_)->GetEventParam(param_idx);
	return id;
}

void JsManager::Return(int id)
{
	auto node = GetCurrentNode();
	auto* chart = static_cast<JsChart*>(node->GetChart());
	chart->Return(id);
}

