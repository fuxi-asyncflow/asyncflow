#include "export_class.h"
#include "py_common.h"
#include "structmember.h"
#include "py_agent.h"
#include "py_manager.h"
#include "export_python.h"
using namespace asyncflow::py;

std::unordered_map<std::string, PyTypeObject> type_dict;

void asyncflow::py::BasicObject_dealloc(BasicObject* self)
{
	Py_TYPE(self)->tp_free((PyObject*)self);
}

PyMemberDef BasicObject_members[] = {
	{"data_ptr", T_LONGLONG, offsetof(BasicObject, data_ptr), 0, "data_ptr"},
	{nullptr}
};

void asyncflow::py::InitCustomPyObj(PyObject* m)
{
	InitManagerType();
	InitAgentType();
	InitChartType();
}

PyObject* asyncflow::py::CreateCustomPyObj(const std::string& cls_name, void* data_ptr)
{
	BasicObject* object;
	if (type_dict.find(cls_name) == type_dict.end())
	{
		ASYNCFLOW_WARN("Can not create {0} object in python", cls_name);
		Py_RETURN_NONE;
	}
	object = PyObject_New(BasicObject, &type_dict[cls_name]);
	object->data_ptr = reinterpret_cast<long long>(data_ptr);
	return (PyObject*)object;
}

PyMethodDef ManagerMethod[] = {
	{"register", (PyCFunction)mgr_register_obj, METH_VARARGS, "mgr_register_obj"},
	{"import_charts", (PyCFunction)mgr_import_charts, METH_VARARGS, "mgr_import_charts"	},
	{"import_event", (PyCFunction)mgr_import_event, METH_VARARGS, "mgr_import_event"},
	{"step", (PyCFunction)mgr_step, METH_VARARGS, "mgr_step"},
	{"event", (PyCFunction)mgr_event, METH_VARARGS, "mgr_event"},
	{"deregister", (PyCFunction)mgr_deregister_obj, METH_VARARGS, "mgr_deregister_obj"},
	{"get_agent", (PyCFunction)mgr_get_agent, METH_VARARGS, "mgr_get_agent"},
	{NULL}  /* Sentinel */
};

void asyncflow::py::InitManagerType()
{
	PyTypeObject ManagerType = { PyObject_HEAD_INIT(NULL) };
	ManagerType.tp_name = "asyncflow.Agent";
	ManagerType.tp_basicsize = sizeof(BasicObject);
	ManagerType.tp_members = BasicObject_members;
	ManagerType.tp_methods = ManagerMethod;
	type_dict["manager"] = ManagerType;
	if (PyType_Ready(&type_dict["manager"]) < 0)
		ASYNCFLOW_ERR("Export manager type fail");
	else
		Py_INCREF(&type_dict["manager"]);
}

PyObject* asyncflow::py::mgr_register_obj(BasicObject* self, PyObject* args)
{
	PyObject* obj;
	int time_interval = Manager::DEFAULT_AGENT_TICK;
	if (!PyArg_ParseTuple(args, "O|i", &obj, &time_interval))
		PY_ARG_ERR;
	auto mgr = reinterpret_cast<PyManager*>(self->data_ptr);
	auto* agent = mgr->RegisterGameObject(obj, time_interval);
	if (agent == nullptr)
	{
		ASYNCFLOW_ERR("create agent failed in asyncflow.register");
		Py_RETURN_NONE;
	}
	return CreateCustomPyObj("agent", agent);
}

PyObject* asyncflow::py::mgr_deregister_obj(BasicObject* self, PyObject* args)
{
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj))
		PY_ARG_ERR;
	auto mgr = reinterpret_cast<PyManager*>(self->data_ptr);
	const auto result = mgr->UnregisterGameObject(obj);
	return PyBool_FromLong(result);
}

PyObject* asyncflow::py::mgr_import_charts(BasicObject* self, PyObject* args)
{
	char* path;
	if (!PyArg_ParseTuple(args, "s", &path))
		PY_ARG_ERR;
	auto mgr = reinterpret_cast<PyManager*>(self->data_ptr);
	const auto result = mgr->ImportFile(path);
	return PyLong_FromLong(result);
}

PyObject* asyncflow::py::mgr_import_event(BasicObject* self, PyObject* args)
{
	char* path;
	if (!PyArg_ParseTuple(args, "s", &path))
		PY_ARG_ERR;

	auto mgr = reinterpret_cast<PyManager*>(self->data_ptr);
	int result = mgr->ImportEvent(path);
	if (result)
	{
		//update eventId table
		PyDict_Clear(EventIdType.tp_dict);
		std::vector<EventInfo> event_list = mgr->GetEventManager().GetEventList();
		PyObject* id = nullptr;
		for (auto event_info : event_list)
		{
			id = PyLong_FromLong(event_info.id);
			PyDict_SetItemString(EventIdType.tp_dict, event_info.name.c_str(), id);
			FreeObject(id);
		}
	}
	return PyLong_FromLong(result);
}

PyObject* asyncflow::py::mgr_step(BasicObject* self, PyObject* args)
{
	int i;
	if (!PyArg_ParseTuple(args, "i", &i))
		PY_ARG_ERR;

	auto mgr = reinterpret_cast<PyManager*>(self->data_ptr);
	mgr->Step(i);
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::mgr_event(BasicObject* self, PyObject* args)
{
	PyObject* obj;
	int id;
	PyObject** event_args;
	int args_count = (int)PyTuple_GET_SIZE(args);
	if (args_count < 2) Py_RETURN_FALSE;

	obj = PyTuple_GetItem(args, 0);
	id = PyLong_AsLong(PyTuple_GetItem(args, 1));

	event_args = new PyObject * [args_count - 2];
	for (int i = 0; i < args_count - 2; i++)
	{
		event_args[i] = PyTuple_GetItem(args, i + 2);
	}
	auto mgr = reinterpret_cast<PyManager*>(self->data_ptr);
	mgr->Event(id, obj, event_args, args_count - 2);
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::mgr_get_agent(BasicObject* self, PyObject* args)
{
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj))
		PY_ARG_ERR;
	auto mgr = reinterpret_cast<PyManager*>(self->data_ptr);
	auto* agent = mgr->agent_manager_.GetAgent(obj);
	if (agent == nullptr)
	{
		ASYNCFLOW_ERR("This obj has not registered in asyncflow");
		Py_RETURN_NONE;
	}
	return CreateCustomPyObj("agent", agent);
}

PyMethodDef AgentMethod[] = {
	{"attach", (PyCFunction)agent_attach, METH_VARARGS, "agent_attach"},
	{"remove", (PyCFunction)agent_remove, METH_VARARGS, "agent_remove"},
	{"get_charts", (PyCFunction)agent_get_charts, METH_VARARGS, "agent_get_charts"},
	{"start", (PyCFunction)agent_start, METH_NOARGS, "agent_start"	},
	{"stop", (PyCFunction)agent_stop, METH_NOARGS, "agent_stop"},
	{"get_obj", (PyCFunction)agent_get_obj, METH_NOARGS, "agent_get_obj"},
	{"get_chart", (PyCFunction)agent_get_chart, METH_VARARGS, "agent_get_chart"},
	{NULL}  /* Sentinel */
};

void asyncflow::py::InitAgentType()
{
	PyTypeObject AgentType = { PyObject_HEAD_INIT(NULL) };
	AgentType.tp_name = "asyncflow.Agent";
	AgentType.tp_basicsize = sizeof(BasicObject);
	AgentType.tp_members = BasicObject_members;
	AgentType.tp_methods = AgentMethod;
	type_dict["agent"] = AgentType;
	if (PyType_Ready(&type_dict["agent"]) < 0)
		ASYNCFLOW_ERR("Export agent type fail");
	else
		Py_INCREF(&type_dict["agent"]);
}

PyObject* asyncflow::py::agent_attach(BasicObject* self, PyObject* args)
{
	char* path;
	PyObject* params = Py_None;
	if (!PyArg_ParseTuple(args, "s|O", &path, &params))
		PY_ARG_ERR;
	auto agent = reinterpret_cast<PyAgent*>(self->data_ptr);
	auto* manager = dynamic_cast<PyManager*>(agent->GetManager());
	auto chart = (PyChart*)(manager->Manager::AttachChart(agent, path));
	if (chart == nullptr)
	{
		ASYNCFLOW_WARN("attach chart failed\n");
		Py_RETURN_FALSE;
	}

	if (PyDict_Check(params))
	{
		std::map<std::string, PyObject*> arg_map;
		PyObject* key, * value;
		Py_ssize_t pos = 0;
		while (PyDict_Next(params, &pos, &key, &value))
		{
			const char* name = PyUnicode_AsUTF8(key);
			arg_map[std::string(name)] = value;
		}
		chart->SetInitTable(params);
	}
	return CreateCustomPyObj("chart", chart);
}

PyObject* asyncflow::py::agent_remove(BasicObject* self, PyObject* args)
{
	char* chart_name;
	if (!PyArg_ParseTuple(args, "s", &chart_name))
		PY_ARG_ERR;

	auto* agent = reinterpret_cast<PyAgent*>(self->data_ptr);
	const auto result = agent->RemoveChart(chart_name);
	return PyBool_FromLong(result);
}

PyObject* asyncflow::py::agent_get_charts(BasicObject* self, PyObject* args)
{
	auto* agent = reinterpret_cast<PyAgent*>(self->data_ptr);
	int is_subchart = 0;
	if (!PyArg_ParseTuple(args, "|p", &is_subchart))
		PY_ARG_ERR;
	const auto& chart_dict = agent->GetChartDict();
	auto py_dict = PyList_New(0);
	PyObject* chart_name = nullptr;
	for (const auto& charts : chart_dict)
	{
		if (is_subchart)
		{
			chart_name = PyUnicode_FromString(charts.first.c_str());
			PyList_Append(py_dict, chart_name);
		}
		else
		{
			for (auto chart : charts.second)
			{
				if (chart->GetOwnerNode() == nullptr)
				{
					chart_name = PyUnicode_FromString(charts.first.c_str());
					PyList_Append(py_dict, chart_name);
					break;
				}
			}
		}
	}
	return py_dict;
}

PyObject* asyncflow::py::agent_start(BasicObject* self, PyObject* args)
{
	auto agent = reinterpret_cast<PyAgent*>(self->data_ptr);
	agent->Start();
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::agent_stop(BasicObject* self, PyObject* args)
{
	auto agent = reinterpret_cast<PyAgent*>(self->data_ptr);
	agent->Stop();
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::agent_get_obj(BasicObject* self, PyObject* args)
{
	auto agent = reinterpret_cast<PyAgent*>(self->data_ptr);
	auto obj = agent->GetRefObject();
	Py_INCREF(obj);
	return obj;
}

PyObject* asyncflow::py::agent_get_chart(BasicObject* self, PyObject* args)
{
	char* path;
	if (!PyArg_ParseTuple(args, "s", &path))
		PY_ARG_ERR;
	auto agent = reinterpret_cast<PyAgent*>(self->data_ptr);
	auto chart = agent->FindChart(path, nullptr);
	if (chart == nullptr)
		Py_RETURN_NONE;
	return CreateCustomPyObj("chart", chart);
}

PyMethodDef ChartMethod[] = {
	{"set_callback", (PyCFunction)chart_set_callback, METH_VARARGS, "set_callback"},
	{NULL}  /* Sentinel */
};

void asyncflow::py::InitChartType()
{
	PyTypeObject ChartType = { PyObject_HEAD_INIT(NULL) };
	ChartType.tp_name = "asyncflow.Chart";
	ChartType.tp_basicsize = sizeof(BasicObject);
	ChartType.tp_members = BasicObject_members;
	ChartType.tp_methods = ChartMethod;
	type_dict["chart"] = ChartType;
	if (PyType_Ready(&type_dict["chart"]) < 0)
		ASYNCFLOW_ERR("Export chart type fail");
	else
		Py_INCREF(&type_dict["chart"]);
}

PyObject* asyncflow::py::chart_set_callback(BasicObject* self, PyObject* args)
{
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj) || !PyCallable_Check(obj))
		PY_ARG_ERR;
	auto chart = self->data_ptr;
	reinterpret_cast<PyChart*>(chart)->SetCall(obj);
	Py_RETURN_TRUE;
}