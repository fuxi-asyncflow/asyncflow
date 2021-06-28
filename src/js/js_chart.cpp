#include "js_chart.h"
#include "export_js.h"
#include "js_manager.h"

using namespace asyncflow::js;

JsChart::~JsChart()
{
	ClearVariables();
	JsManager* mgr = (JsManager *) GetAgent()->GetManager();
#ifdef FLOWCHART_DEBUG
	mgr->GetJsDebugger().StopDebugChartWithData(this);
#endif
}

bool JsChart::InitArgs()
{
	auto var_count = data_->GetVarCount();
	variables_ = std::vector<int>(var_count);
	JsManager* mgr = (JsManager*)GetAgent()->GetManager();
#ifdef FLOWCHART_DEBUG
	mgr->GetJsDebugger().StartQuickDebug(this);
#endif
	return true; 
}

void JsChart::SetArgs(void* args, int argc)
{
	int* js_args = reinterpret_cast<int*>(args);
	int param_count = data_->GetVarCount();
	int count = param_count < argc ? param_count : argc;
	for (int i = 0; i < count; i++)
	{
		SetVar(i, js_args[i]);
	}
}

void JsChart::ClearVariables()
{
	for (auto var : variables_)
	{
		if (var != 0)
			js_clear_variable(var);
	}
}

void JsChart::ResetVariables()
{
	ClearVariables();
	InitArgs();
}

void JsChart::SetVar(int idx, int id)
{
	if (idx >= variables_.size() || idx < 0)
	{
		ASYNCFLOW_WARN("SetVar error! The idx is out of range!");
		return;
	}
	variables_[idx] = id;
}

int JsChart::GetVar(int idx)
{
	if (idx >= variables_.size() || idx < 0)
	{
		ASYNCFLOW_WARN("GetVar error! The idx is out of range!");
		return -1;
	}
	return variables_[idx];
}

void JsChart::Return(int idx)
{
	if (status_ != Running)
		return;
	int result = js_is_true(idx);
	if (result == -1)
	{
		ASYNCFLOW_WARN("Js variable_dict does not contain this id {0}!", idx);
	}
	auto owner_node = this->GetOwnerNode();
	if (owner_node != nullptr)
	{
		auto var_id = owner_node->GetData()->GetVarId();
		if (var_id >= 0)
			((JsChart*)(owner_node->GetChart()))->SetVar(var_id, idx);
	}
	Chart::Return(result);
	if (callback_)
		js_callback(idx, data_->Name().c_str());
}

void JsChart::Return(bool idx)
{
	Chart::Return(true);
	if (callback_)
		js_callback(idx, data_->Name().c_str());
}

#ifdef FLOWCHART_DEBUG
void JsChart::SendEventStatus(std::string node_uid, const AsyncEventBase* event)
{
	auto* py_event = (AsyncEvent*)event;
	std::vector<std::string> args;
	for (int i = 0; i < py_event->argCount(); i++)
	{
		args.push_back(js_to_string(py_event->GetEventParam(i)));
	}
	auto event_name = agent_->GetManager()->GetEventManager().GetEventName(py_event->Id());
	debug::EventStatusData* data = new debug::EventStatusData(node_uid, event_name, args, py_event->argCount());
	data->id = debug_data_count_++;
	debug_data_list_.push_back(data);
}
#endif

