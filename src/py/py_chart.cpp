#include "py_chart.h"
#include "export_class.h"
#include "py_manager.h"
#include "py_common.h"
#include "weakrefobject.h"

///////////////////////////////////////////////////////////////////////////////////////////
asyncflow::py::PyChart::PyChart(Manager* mgr)
	:Chart(mgr), variables_(nullptr), call_(Py_None), init_table_(nullptr)
{
	Py_IncRef(Py_None);
	export_object_ = ChartObject::New(this);
}

asyncflow::py::PyChart::~PyChart()
{
	ClearVariables();
	Py_XDECREF(call_);
	Py_XDECREF(init_table_);
	PyObjectRefHelper::DecRef(export_object_);
	if(variables_ != nullptr)
	{
		const int var_count = data_->GetVarCount();
		for (int i = 0; i < var_count; i++)
		{
			Py_XDECREF(variables_[i]);
		}
		delete[] variables_;
		variables_ = nullptr;
	}
}


PyObject* asyncflow::py::PyChart::GetVar(int id, bool weak)
{
	if (id > data_->GetVarCount() || id < 0)
	{
		ASYNCFLOW_WARN("var id {0} is out of range in chart {1}", id, Name());
		Py_IncRef(Py_None);
		return Py_None;
	}

	auto* var_obj = variables_[id];
	if (weak && PyWeakref_Check(var_obj))
		var_obj = PyWeakref_GetObject(var_obj);

	Py_XINCREF(var_obj);
	return var_obj;
}

void asyncflow::py::PyChart::InvokeCallback(PyObject* obj)
{
	if (PyCallable_Check(call_))
	{
		auto* temp_call = call_;
		call_ = Py_None;
		Py_IncRef(Py_None);
		PyObject_CallFunctionObjArgs(temp_call, obj, nullptr);
		if (CheckPythonException())
		{
			ASYNCFLOW_ERR("error in callback function of chart {0}", Name());
		}
		Py_DecRef(temp_call);
	}
}

void asyncflow::py::PyChart::Return(PyObject* obj)
{
	//TODO Because some nodes have added to the handling_nodes_list before chart was stopped,
	//So some nodes may continue to run after stopping the chart
	if (status_ != Running)
		return;
	bool result = ObjIsBool(obj);
	auto owner_node = this->GetOwnerNode();;
	if (owner_node != nullptr)
	{
		auto var_id = owner_node->GetData()->GetVarId();
		if (var_id >= 0)
			((PyChart*)(owner_node->GetChart()))->SetVar(var_id, obj);
	}

	Chart::Return(result);
	InvokeCallback(obj);
}

void asyncflow::py::PyChart::Return(bool result)
{
	Chart::Return(result);
	InvokeCallback(PyBool_FromLong(result));
}

void asyncflow::py::PyChart::SetCall(PyObject* call)
{
	//TODO callback function may change
	Py_XDECREF(call_);
	call_ = call;
	Py_XINCREF(call_);
}

void asyncflow::py::PyChart::SetInitTable(PyObject* tb)
{
	Py_XDECREF(init_table_);
	init_table_ = tb;
	Py_XINCREF(init_table_);
}

//index starts from 1 as lua table
bool asyncflow::py::PyChart::SetVar(int id, PyObject* v, bool weak)
{
	if (id > data_->GetVarCount() || id < 0)
	{
		ASYNCFLOW_WARN("var id {0} is out of range in chart {1}", id, Name());
		return false;
	}
#ifdef FLOWCHART_DEBUG
	SendVariableStatus(data_->GetVariableName(id), variables_[id], v);
#endif
	Py_XDECREF(variables_[id]);
	if(weak)
	{
		v = PyWeakref_NewRef(v, nullptr);
	}

	variables_[id] = v;
	Py_XINCREF(v);
	return true;
}

void asyncflow::py::PyChart::ClearVariables()
{
	int var_count = data_->GetVarCount();
	if (variables_ != nullptr)
	{
		for (int i = 0; i < var_count; i++)
		{
			Py_XDECREF(variables_[i]);
            variables_[i] = Py_None;
			Py_INCREF(Py_None);
		}
	}
}

void asyncflow::py::PyChart::ResetVariables()
{
	InitArgs();
	if (init_table_ != nullptr)
	{
		std::map<std::string, PyObject*> arg_map;
		PyObject* key, * value;
		Py_ssize_t pos = 0;
		while (PyDict_Next(init_table_, &pos, &key, &value))
		{
			const char* name = PyUnicode_AsUTF8(key);
			arg_map[std::string(name)] = value;
		}
		SetArgsFromDict(arg_map);
	}
}


void asyncflow::py::PyChart::SetArgs(void* args, int argc)
{
	PyObject** pyargs = reinterpret_cast<PyObject**>(args);
	int param_count = data_->GetVarCount();
	int count = param_count < argc ? param_count : argc;
	for (int i = 0; i < count; i++)
	{
		SetVar(i, pyargs[i]);
	}
}

bool asyncflow::py::PyChart::InitArgs()
{
	auto var_count = data_->GetVarCount();
	if (variables_ == nullptr)
	{
		variables_ = new PyObject *[var_count];
		for (int i = 0; i < var_count; i++)
		{
			variables_[i] = Py_None;
			Py_INCREF(Py_None);
		}
	}
	else
	{
		for (int i = 0; i < var_count; i++)
		{
			auto* v = variables_[i];
			if (v != Py_None)
			{
				Py_XDECREF(v);
				variables_[i] = Py_None;
				Py_INCREF(Py_None);
			}
		}
	}
	return true;
}

void asyncflow::py::PyChart::SetArgsFromDict(const std::map<std::string, PyObject*>& dict)
{
	PyChartData* data = dynamic_cast<PyChartData*>(data_);
	if (data == nullptr)
	{
		ASYNCFLOW_ERR("chart data should be python chart data");
		return;
	}
	auto& var_info = data->GetVariableInfo();
	auto param_count = data->GetParamCount();
	for (auto i = 0; i < param_count; ++i)
	{
		auto& param = var_info[i];
		auto it = dict.find(param.name);
		if (it != dict.end())
		{
			PyObject* value = nullptr;
			//Change type according to type of setting
			if (param.type == "Bool" && !PyBool_Check(it->second))
			{
				const char* valueC = PyUnicode_AsUTF8(it->second);
				value = !strcmp(valueC, "true") ? Py_True : Py_False;
				SetVar(i, value);
			}
			else if (param.type == "Number" && !PyNumber_Check(it->second))
			{
#ifdef USING_PYTHON2
				value = PyFloat_FromString(it->second, nullptr);
#else
				value = PyFloat_FromString(it->second);
#endif
				SetVar(i, value);
				Py_XDECREF(value);
			}
			else
			{
				SetVar(i, it->second);
			}
		}
		else
		{
			ASYNCFLOW_WARN("chart {0} lack parameter {1}", data_->Name(), param.name);
		}
	}
}

#ifdef FLOWCHART_DEBUG
void asyncflow::py::PyChart::SendEventStatus(std::string node_uid, const AsyncEventBase* event)
{
	auto* py_event = (AsyncEvent*)event;
	std::vector<std::string> args;
	for (int i = 0; i < py_event->argCount(); i++)
	{
		args.push_back(ToString(py_event->GetEventParam(i)));
	}
	auto event_name = agent_->GetManager()->GetEventManager().GetEventName(py_event->Id());
	debug::EventStatusData* data = new debug::EventStatusData(node_uid, event_name, args, py_event->argCount());
	data->id = debug_data_count_++;
	debug_data_list_.push_back(data);
}

void asyncflow::py::PyChart::SendVariableStatus(std::string var_name, PyObject* old_value, PyObject* new_value)
{
	if (!debug_)
		return;
	if (old_value != new_value)
	{
		auto current_uid = agent_->GetManager()->GetCurrentNode()->GetData()->GetUid();
		debug::VariableStatusData* data = new debug::VariableStatusData(var_name, current_uid, ToString(old_value), ToString(new_value));
		data->id = debug_data_count_++;
		debug_data_list_.push_back(data);
	}
}
#endif
