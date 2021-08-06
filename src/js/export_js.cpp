#include "export_js.h"
#include <stdio.h>
#include "js_manager.h"
#include "util/log.h"
#include "core/node_data.h"
using Log = asyncflow::util::Log;
using namespace asyncflow::core;
using namespace asyncflow::js;

static JsManager* mgr = nullptr;
static const char* manager_null_msg = "manager is null, any function shoud called after asyncflow.setup()";

EM_PORT_API(int) setup()
{
	asyncflow::util::Log::Init();
	if (mgr == nullptr)
	{
		mgr = new JsManager();
		return 1;
	}
	return 0;
}

EM_PORT_API(int) import_charts(char* str)
{
	if (mgr == nullptr)
	{
		ASYNCFLOW_WARN(manager_null_msg);
		return 0;
	}
	int counts = mgr->ImportJson(std::string(str));
	return counts;
}

EM_PORT_API(int) import_event(char* str)
{
	if (mgr == nullptr)
	{
		ASYNCFLOW_WARN(manager_null_msg);
		return 0;
	}
	int result = mgr->ImportEvent(str);
	return result;
}

EM_PORT_API(int) register_obj(int id, int tick)
{
	if (mgr == nullptr)
	{
		ASYNCFLOW_WARN(manager_null_msg);
		return 0;
	}
	auto agent = mgr->RegisterGameObject(id, tick);
	if (agent == nullptr)
	{
		ASYNCFLOW_ERR("create agent failed in asyncflow.register");
		return 0;
	}
	return 1;
}

EM_PORT_API(int) deregister_obj(int id)
{
	if (mgr == nullptr)
	{
		ASYNCFLOW_WARN(manager_null_msg);
		return 0;
	}
	int result = mgr->UnregisterGameObject(mgr->GetAgent(id));
	js_clear_gameobj(id);
	return result;
}

EM_PORT_API(int) asyncflow_exit()
{
	if (mgr == nullptr)
		return 0;
	delete mgr;
	mgr = nullptr;
	return 1;
}

EM_PORT_API(void *) attach(int id, char* str)
{
	if (mgr == nullptr)
	{
		ASYNCFLOW_WARN(manager_null_msg);
		return 0;
	}
	auto chart = mgr->AttachChart(id, str);
	if (chart == nullptr)
		return nullptr;
	return chart;
}

EM_PORT_API(int) remove_chart(int id, char* str)
{
	if (mgr == nullptr)
	{
		ASYNCFLOW_WARN(manager_null_msg);
		return 0;
	}
	auto agent = mgr->GetAgent(id);
	if (agent == nullptr)
	{
		ASYNCFLOW_WARN("object is not registered to asyncflow");
		return 0;
	}
	bool result = agent->RemoveChart(str);
	return result;
}

EM_PORT_API(int) callback(void * chart_ptr)
{
	auto chart = reinterpret_cast<JsChart *>(chart_ptr);
	chart->SetCall();
	return 1;
}

EM_PORT_API(int) start(int id)
{
	if (mgr == nullptr)
	{
		ASYNCFLOW_WARN(manager_null_msg);
		return 0;
	}
	auto agent = mgr->GetAgent(id);
	if (agent == nullptr)
	{
		ASYNCFLOW_WARN("object is not registered to asyncflow");
		return 0;
	}
	agent->Start();
	return 1;
}


EM_PORT_API(int) stop(int id)
{
	if (mgr == nullptr)
	{
		ASYNCFLOW_WARN(manager_null_msg);
		return 0;
	}
	auto agent = mgr->GetAgent(id);
	if (agent == nullptr)
	{
		ASYNCFLOW_WARN("object is not registered to asyncflow");
		return 0;
	}
	agent->Stop();
	return 1;
}


EM_PORT_API(int) start_chart(int id, char* chart_name)
{
	if (mgr == nullptr)
	{
		ASYNCFLOW_WARN(manager_null_msg);
		return 0;
	}
	auto agent = mgr->GetAgent(id);
	if (agent == nullptr)
	{
		ASYNCFLOW_WARN("object is not registered to asyncflow");
		return 0;
	}
	agent->StartChart(chart_name);
	return 1;
}


EM_PORT_API(int) stop_chart(int id, char* chart_name)
{
	if (mgr == nullptr)
	{
		ASYNCFLOW_WARN(manager_null_msg);
		return 0;
	}
	auto agent = mgr->GetAgent(id);
	if (agent == nullptr)
	{
		ASYNCFLOW_WARN("object is not registered to asyncflow");
		return 0;
	}
	agent->StopChart(chart_name);
	return 1;
}

EM_PORT_API(int) step(int time)
{
	if (mgr == nullptr)
	{
		ASYNCFLOW_WARN(manager_null_msg);
		return 0;
	}
	mgr->Step(time);
	return 1;
}

EM_PORT_API(int) event(int obj_id, int  event_id, uint8_t* ptr, int count)
{
	if (mgr == nullptr)
	{
		ASYNCFLOW_WARN(manager_null_msg);
		return 0;
	}
	if (count < 0)
	{
		ASYNCFLOW_WARN("the event arg count must be larger than 0");
		return 0;
	}
	int* args = new int[count];
	for (int i = 0; i < count; i++) {
		memcpy(args + i, ptr + i * 4, 4);
	}
	bool result = mgr->Event(event_id, obj_id, args, count);
	return result;
}

EM_PORT_API(int) debug_command(char* str)
{
	if (mgr == nullptr)
	{
		return 0;
	}
#ifdef FLOWCHART_DEBUG
	mgr->OnMessage(str);
	return 1;
#endif
	ASYNCFLOW_WARN("please define FLOWCHART_DEBUG");
	return 0;
}

#pragma region asyncflow_inner_func
EM_PORT_API(int) wait(float interval)
{
	if (mgr == nullptr)
	{
		return 0;
	}
	mgr->Wait(1000.0 *interval);
	return 1;
}

EM_PORT_API(int) get_var(int idx)
{
	if (mgr == nullptr)
	{
		return 0;
	}
	int id = mgr->GetVar(idx);
	return id;
}

EM_PORT_API(int) set_var(int idx, int id)
{
	if (mgr == nullptr)
	{
		return 0;
	}
	mgr->SetVar(idx, id);
	return 1;
}

EM_PORT_API(int) call_sub(char* chart_name, int id, uint8_t* ptr, int count)
{
	if (mgr == nullptr)
	{
		return 0;
	}
	if (count < 0)
		return 0;
	int* args = new int[count];
	for (int i = 0; i < count; i++) {
		memcpy(args + i, ptr + i * 4, 4);
	}
	mgr->Subchart(chart_name, id, args, count);
	return 1;
}

EM_PORT_API(int) ret(int id)
{
	if (mgr == nullptr)
	{
		return 0;
	}
	mgr->Return(id);
	return 1;
}

EM_PORT_API(int) wait_event(int id, int event_id)
{
	if (mgr == nullptr)
	{
		return 0;
	}
	if (event_id < 1)
	{
		ASYNCFLOW_ERR("event_id can not be {0}", event_id);
		return 0;
	}
	Agent* agent = mgr->GetAgent(id);
	if (agent == nullptr)
	{
		ASYNCFLOW_ERR("wait event obj {0} is not registered", id);
		agent = mgr->RegisterGameObject(id, Manager::DEFAULT_AGENT_TICK);
	}
	auto res = mgr->WaitEvent(agent, event_id);
	return res;
}

EM_PORT_API(int) get_event_param(int event_id, int param_id)
{
	if (mgr == nullptr)
	{
		return 0;
	}
	int id = mgr->GetEventPatam(event_id, param_id);
	return id;
}

EM_PORT_API(int) wait_all(uint8_t* ptr, int count)
{
	if (mgr == nullptr)
	{
		return 0;
	}
	int* args = new int[count];
	for (int i = 0; i < count; i++) {
		memcpy(args + i, ptr + i * 4, 4);
	}
	bool result = mgr->WaitAll(span<int>(args, count));
	delete[] args;
	return 0;
}

EM_PORT_API(int) get_time()
{
	if (mgr == nullptr)
	{
		return 0;
	}
	return mgr->Now();
}

#pragma endregion asyncflow_inner_func