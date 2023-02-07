#include "py_manager.h"
#include "py_chartdata.h"
#include "py_chart.h"

using namespace asyncflow::core;
using namespace asyncflow::py;

/////////////////////////////////////////////////////////////////////////////////////

bool PyManager::UnregisterGameObject(PyObject* obj)
{
	auto agent = agent_manager_.GetAgent(obj);
	return Manager::UnregisterGameObject(agent);
}

bool PyManager::Event(int event_id, PyObject* obj, PyObject** args, int arg_count, bool immediate)
{
	return Manager::Event(event_id, agent_manager_.GetAgent(obj), (void*)args, arg_count, immediate);
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

bool PyManager::AsyncCallback(long long context, PyObject* v)
{
	auto node = (core::Node*)context;
	if (async_manager_.IsNodeWaiting(node))
	{
		ASYNCFLOW_DBG("async callback for node {0} {1}[{2}]", (void*)node, node->GetChart()->Name(), node->GetId());
		async_manager_.ActivateNode(node);
		auto const var_id = node->GetData()->GetVarId();
		if (var_id >= 0)
			((PyChart*)node->GetChart())->SetVar(var_id, v);
		bool ret = ObjIsBool(v);
		node->SetResult(ret);
	}
	else
	{
		//ASYNCFLOW_WARN("node {0} {1}[{2}] is not waiting async callback", (void*)node, node->GetChart()->Name(), node->GetId());
		ASYNCFLOW_WARN("node {0} is not waiting async callback", (void*)node);
	}
	return true;
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

