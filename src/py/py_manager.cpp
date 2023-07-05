#include "py_manager.h"
#include "py_chartdata.h"
#include "py_chart.h"
#include "export_class.h"

using namespace asyncflow::core;
using namespace asyncflow::py;

PyManager* PyManager::current_manager_ = nullptr;

/////////////////////////////////////////////////////////////////////////////////////
///
PyManager::PyManager()
: Manager()
, agent_manager_(this)
{
	export_object_ = ManagerObject::New(this);
}

PyManager::~PyManager()
{
	auto* obj = (ManagerObject*)(export_object_);
	obj->ptr = nullptr;
	PyObjectRefHelper::DecRef(export_object_);
	if (current_manager_ == this)
		current_manager_ = nullptr;
}

AsyncEventBase* PyManager::CreateAsyncEvent(int event_id, Agent* agent, void* args, int arg_count)
{
	auto event = new AsyncEvent(event_id, agent);
	event->SetArgs((PyObject**)args, arg_count);
	return event;
}

Agent* PyManager::RegisterGameObject(PyObject* obj, int tick_interval)
{
	if (tick_interval <= 0)
		tick_interval = DEFAULT_AGENT_TICK;
	if (agent_manager_.GetAgent(obj) != nullptr)
	{
		ASYNCFLOW_LOG("object has registered to asyncflow");
		return nullptr;
	}
	auto agent = agent_manager_.Register(obj);
	if (tick_interval != Manager::default_time_interval_)
		agent->SetTickInterval(tick_interval);
	return agent;
}

bool PyManager::UnregisterGameObject(PyObject* obj)
{
	auto agent = agent_manager_.GetAgent(obj);
	return Manager::UnregisterGameObject(agent);
}

bool PyManager::Event(int event_id, PyObject* obj, PyObject** args, int arg_count, bool immediate, bool trigger)
{
	return Manager::Event(event_id, agent_manager_.GetAgent(obj), (void*)args, arg_count, immediate, trigger);
}

bool PyManager::SetVar(int var_id, PyObject* obj)
{
	auto node = GetCurrentNode();
	return ((PyChart*)node->GetChart())->SetVar(var_id, obj);
}

PyObject* PyManager::GetVar(int var_id)
{
	auto node = GetCurrentNode();
	return ((PyChart*)node->GetChart())->GetVar(var_id);
}

PyObject* PyManager::GetEventParam(int event_id, int param_idx)
{
	assert(current_event_ != nullptr);
	if (event_id != current_event_->Id())
	{
		ASYNCFLOW_WARN("event id unmatch when get event param");
	}
	return ((AsyncEvent*)current_event_)->GetEventParam(param_idx);
}

bool PyManager::Return(PyObject* ret_val)
{
	auto node = GetCurrentNode();
	PyChart* chart = (PyChart*)node->GetChart();
	chart->Return(ret_val);
	return ObjIsBool(ret_val);
}

bool PyManager::ContinueAsyncNode(int64_t context, PyObject* v)
{
	auto result = Manager::ContinueAsyncNode(context);	
	if (result)
	{
		auto* node = reinterpret_cast<Node*>(context);
		auto const var_id = node->GetData()->GetVarId();
		if (var_id >= 0)
			((PyChart*)node->GetChart())->SetVar(var_id, v);
		bool ret = ObjIsBool(v);
		node->SetResult(ret);
	}	
	return result;
}

std::pair<bool, std::vector<std::string>> PyManager::RunScript(const char* str)
{
	std::vector<std::string> result;
	auto* py_global = PyEval_GetGlobals();
	auto res = PyRun_String(str, Py_file_input, py_global, py_global);
	if (CheckPythonException() || res == nullptr)
	{
		result.push_back("run python script error");
		return std::pair<bool, std::vector<std::string>>(false, result);
	}
	result.push_back(ToString(res));
	Py_XDECREF(res);
	return std::pair<bool, std::vector<std::string>>(true, result);
}

bool PyManager::Subchart(std::string chart_name, PyObject* obj, PyObject** args, int arg_count)
{
	auto* agent = agent_manager_.GetAgent(obj);
	//If the obj does not register then create a new one, its tick_interval as default
	if (agent == nullptr)
	{
		ASYNCFLOW_ERR("subchart object {0} is not registered", (void*)obj);
		agent = agent_manager_.Register(obj);
	}

	return Manager::Subchart(chart_name, agent, args, arg_count);
}

bool PyManager::StartAgent(PyObject* obj)
{
	auto* agent = agent_manager_.GetAgent(obj);
	if (agent == nullptr)
		return false;

	agent->Start();
	return true;
}

Chart* PyManager::AttachChart(PyObject* obj, const std::string& chart_name)
{
	auto* agent = agent_manager_.Register(obj);
	return Manager::AttachChart(agent, chart_name);
}

