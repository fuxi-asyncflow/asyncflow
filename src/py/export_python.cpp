#include "py_manager.h"
#include "py_agent.h"
#include "export_python.h"
#include "Python.h"
#include "util/log.h"
#include "core/manager.h"
#include <map>
#include <vector>
#include "py_chart.h"
#include "python_sink.h"
#include "py_common.h"
#include "export_class.h"

#ifdef FLOWCHART_DEBUG
#include "debug/websocket_manager.h"
using namespace asyncflow::debug;
#endif

using Log = asyncflow::util::Log;
using namespace asyncflow::core;
using namespace asyncflow::py;

PyObject* asyncflow::py::GetModule() { return _module; }

PyTypeObject asyncflow::py::EventIdType = {
	PyObject_HEAD_INIT(NULL)
};

void asyncflow::py::InitEventIdObject(PyObject* module)
{
	EventIdType.tp_name = "asyncflow.EventId";
	EventIdType.tp_basicsize = sizeof(EventIdObject);
	if (PyType_Ready(&EventIdType) < 0)
		return;

	Py_INCREF(&EventIdType);
	PyModule_AddObject(module, "EventId", (PyObject*)&EventIdType);
}

#pragma region asyncflow_customer_func
//TODO support several managers in python
PyObject* asyncflow::py::setup(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
	{
		manager = new PyManager;
		PyManager::SetCurrentManager(manager);
		ASYNCFLOW_LOG("asyncflow setup");
	}
	else
	{
		ASYNCFLOW_LOG("asyncflow already setup");
		Py_RETURN_NONE;
	}
	PyObject* config = Py_None;
	if (PyArg_ParseTuple(args, "|O", &config) && PyDict_Check(config))
	{
		auto result = PyDict_GetItemString(config, "loop_check");
		if (result != nullptr)
		{
			bool flag = PyObject_IsTrue(result);
			manager->GetExecutor().SetLoopCheck(flag);
			ASYNCFLOW_LOG("loop_check is set to {0}", flag);
		}
		result = PyDict_GetItemString(config, "immediate_subchart");
		if (result != nullptr)
		{
			bool flag = PyObject_IsTrue(result);
			manager->SetImmediateSub(flag);
			ASYNCFLOW_LOG("immediate_subchart is set to {0}", flag);
		}
		result = PyDict_GetItemString(config, "default_timestep");
		if (result != nullptr)
		{
			if (PyLong_Check(result))
			{
				int step = (int)PyLong_AsLong(result);
				if (step <= 0)
					ASYNCFLOW_WARN("default_timestep must be larger than 0");
				else
				{
					manager->SetDefaulTimeInterval(step);
					ASYNCFLOW_LOG("default_timestep is set to {0}", step);
				}
			}
			else
			{
				ASYNCFLOW_WARN("default_timestep must be long");
			}
		}
#ifdef FLOWCHART_DEBUG
		std::string ip = WebsocketManager::IP;
		int port = WebsocketManager::START_PORT;
		result = PyDict_GetItemString(config, "debug_ip");
		if (result != nullptr)
		{
			if (PyUnicode_Check(result))
			{
				ip = PyUnicode_AsUTF8(result);
				ASYNCFLOW_LOG("debug_ip is set to {0}", ip);
			}
			else
			{
				ASYNCFLOW_WARN("debug_ip must be string");
			}
		}
		result = PyDict_GetItemString(config, "debug_port");
		if (result != nullptr)
		{
			if (PyLong_Check(result))
			{
				port = (int)PyLong_AsLong(result);
				ASYNCFLOW_LOG("debug_port is set to {0}", port);
			}
			else
			{
				ASYNCFLOW_WARN("debug_port must be long");
			}
		}
		result = PyDict_GetItemString(config, "debug_name_function");
		if (result != nullptr)
		{
			if (PyUnicode_Check(result))
			{
				auto method_name = PyUnicode_AsUTF8(result);
				Agent::DEBUG_NAME_METHOD = method_name;
				ASYNCFLOW_LOG("debug_name_function is set to {0}", Agent::DEBUG_NAME_METHOD);
			}
			else
			{
				ASYNCFLOW_WARN("debug_name_function must be string");
			}
		}
		manager->GetWebsocketManager().Init(ip, port);
#endif
	}
	else
	{
#ifdef FLOWCHART_DEBUG
		manager->GetWebsocketManager().Init(WebsocketManager::IP, WebsocketManager::START_PORT);
#endif
	}
	
	return manager->GetExportObject();
}

PyObject* asyncflow::py::exit(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager != nullptr)
	{
		delete manager;
	}
	CheckPythonException();
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::import_charts(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;

	char* path;
	if (!PyArg_ParseTuple(args, "s", &path))
		PY_ARG_ERR;
	const auto result = manager->ImportFile(path);
	return PyLong_FromLong(result);
}

PyObject* asyncflow::py::patch(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;

	char* path;
	PyObject* in_place_obj = nullptr;
	if (!PyArg_ParseTuple(args, "s|O", &path, &in_place_obj))
		PY_ARG_ERR;
	bool in_place = false;
	if(in_place_obj != nullptr)
        in_place = PyBool_Check(in_place_obj) && PyObject_IsTrue(in_place_obj);
	const auto result = manager->PatchFromYaml(path, in_place);
	return PyLong_FromLong(result);
}

PyObject* asyncflow::py::import_event(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;

	char* path;
	if (!PyArg_ParseTuple(args, "s", &path))
		PY_ARG_ERR;

	int result = manager->ImportEvent(path);
	if (result)
	{
		//update eventId table
		PyDict_Clear(EventIdType.tp_dict);
		std::vector<EventInfo> event_list = manager->GetEventManager().GetEventList();
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

PyObject* asyncflow::py::register_obj(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;

	PyObject* obj;
	int time_interval = Manager::DEFAULT_AGENT_TICK;
	if (!PyArg_ParseTuple(args, "O|i", &obj, &time_interval))
		PY_ARG_ERR;

	auto* agent = manager->RegisterGameObject(obj, time_interval);
	if (agent == nullptr)
	{
		ASYNCFLOW_ERR("create agent failed in asyncflow.register");
		Py_RETURN_NONE;
	}
	
	return ((PyAgent*)agent)->GetExportObject();
}

PyObject* asyncflow::py::get_agent(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;

	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj))
		PY_ARG_ERR;
	
	auto* agent = manager->GetAgent(obj);
	if (agent == nullptr)
	{
		ASYNCFLOW_ERR("This obj has not registered in asyncflow");
		Py_RETURN_NONE;
	}
	return agent->GetExportObject();
}

PyObject* asyncflow::py::deregister_obj(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;

	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj))
		PY_ARG_ERR;

	const auto result = manager->UnregisterGameObject(obj);
	return PyBool_FromLong(result);
}

PyObject* asyncflow::py::get_current_manager(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;
	return manager->GetExportObject();
}

PyObject* asyncflow::py::attach(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;

	PyObject* obj;
	char* path;
	PyObject* params = Py_None;
	//static const char* kwlist[] = { "obj", "chart", "params" };
	if (!PyArg_ParseTuple(args, "Os|O", &obj, &path, &params))
		PY_ARG_ERR;

	auto chart = (PyChart*)manager->AttachChart(obj, path);
	if (chart == nullptr)
	{
		ASYNCFLOW_WARN("attach chart failed\n");
		Py_RETURN_NONE;
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
	return chart->GetExportObject();
}

PyObject* asyncflow::py::remove(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;
	PyObject* obj;
	char* chart_name;
	if (!PyArg_ParseTuple(args, "Os", &obj, &chart_name))
		PY_ARG_ERR;

	auto* agent = manager->GetAgent(obj);
	if (agent == nullptr)
	{
		Py_RETURN_FALSE;
	}
	const auto result = agent->RemoveChart(chart_name);
	return PyBool_FromLong(result);
}

PyObject* asyncflow::py::start(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;
	PyObject* obj;
	PyObject* list = Py_None;
	if (!PyArg_ParseTuple(args, "O|O", &obj, &list) || (!PyList_Check(list) && (list != Py_None)))
		PY_ARG_ERR;
	auto agent = manager->GetAgent(obj);
	if (agent == nullptr)
	{
		return PyLong_FromLong(0);
	}
	if (list == Py_None)
	{
		return PyLong_FromLong(agent->Start());
	}
	auto size = PyList_Size(list);
	int count = 0;
	for (auto i = 0; i < size; i++) {
		auto pItem = PyList_GetItem(list, i);
		//TODO check pItem type, python2 is string, python3 is unicode
		auto chart_name = PyUnicode_AsUTF8(pItem);
		if (agent->StartChart(chart_name))
			count++;		
	}
	return PyLong_FromLong(count);
}

PyObject* asyncflow::py::stop(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;

	PyObject* obj;
	PyObject* list = Py_None;
	if (!PyArg_ParseTuple(args, "O|O", &obj, &list) || (!PyList_Check(list) && (list != Py_None)))
		PY_ARG_ERR;
	auto agent = manager->GetAgent(obj);
	if (agent == nullptr)
	{
		Py_RETURN_FALSE;
	}
	if (list == Py_None)
	{
		agent->Stop();
		Py_RETURN_TRUE;
	}
	auto size = PyList_Size(list);
	for (auto i = 0; i < size; i++) {
		auto pItem = PyList_GetItem(list, i);
		if (PyUnicode_Check(pItem)) {
			auto chart_name = PyUnicode_AsUTF8(pItem);
			agent->StopChart(chart_name);
		}
	}
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::step(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;

	int i;
	if (!PyArg_ParseTuple(args, "i", &i))
		PY_ARG_ERR;

	manager->Step(i);
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::event(PyObject* self, PyObject* args)
{
	return _event(self, args, false);
}

PyObject* asyncflow::py::trigger(PyObject* self, PyObject* args)
{
	return _event(self, args, true);
}

PyObject* asyncflow::py::_event(PyObject* self, PyObject* args, bool trigger)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;
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

	auto ret = manager->Event(id, obj, event_args, args_count - 2, false, trigger);
	return PyBool_FromLong(ret);
}

PyObject* asyncflow::py::get_charts(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj))
		PY_ARG_ERR;
	auto agent = manager->GetAgent(obj);
	auto chart_name_list = agent->GetRunningChartNames();
	auto pylist = PyList_New(chart_name_list.size());
	for (int i = 0; i < chart_name_list.size(); i++)
	{
		PyList_SetItem(pylist, i, PyUnicode_FromStringAndSize(chart_name_list[i].c_str(), chart_name_list[i].size()));
	}
	return pylist;
}

PyObject* asyncflow::py::config_log(PyObject* self, PyObject* args)
{
	char* path;
	char* log_name;
	PyObject* param = nullptr;
	if (!PyArg_ParseTuple(args, "ss", &path, &log_name))
	{
		printf("parse argument failed in config_log !\n");
		Py_RETURN_FALSE;
	}
	Log::SetLog(path, log_name);
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::set_logger(PyObject* self, PyObject* args)
{
	PyObject* python_log = nullptr;
	if (!PyArg_ParseTuple(args, "O", &python_log))
	{
		printf("parse argument failed in set_python_log!\n");
		Py_RETURN_FALSE;
	}
	if (auto log = spdlog::get("python_log"))
	{
		for(auto& sink : log->sinks())
		{
			auto python_sink = std::dynamic_pointer_cast<spdlog::python_logger_sink>(sink);
			if(python_sink)
			{
				python_sink->set(python_log);
			}
		}
	}
	else
	{
		Log::rotatelogger = spdlog::create<spdlog::python_logger_sink>("python_log", python_log);
	}
	Log::rotatelogger->set_level(Log::LEVEL);
	Log::rotatelogger->set_pattern("[asyncflow] %v");
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::set_node_func(PyObject* self, PyObject* args)
{
	//PyObject* async_module = PyImport_ImportModule("asyncflow");
    PyObject* async_module = self;
	if (CheckPythonException() || async_module == nullptr)
	{
		ASYNCFLOW_ERR("cannot find asyncflow module when set_node_func");
		Py_RETURN_FALSE;
	}
	PyObject* func_dict = PyObject_GetAttrString(async_module, "node_funcs");
	if (CheckPythonException() || func_dict == nullptr)
	{
		ASYNCFLOW_ERR("get node_funcs dict error");
		Py_RETURN_FALSE;
	}

	char* name;
	PyObject* func;
	if (!PyArg_ParseTuple(args, "sO", &name, &func))
		PY_ARG_ERR;

	int ret = PyDict_SetItemString(func_dict, name, func);
	Py_DECREF(func_dict);
	if (ret != 0)
	{
		ASYNCFLOW_ERR("set node func {0} wrong", name);
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}
#pragma endregion asyncflow_customer_func

#pragma region asyncflow_inner_func
PyObject* asyncflow::py::wait(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		Py_RETURN_FALSE;
	auto arg0 = PyTuple_GetItem(args, 0);
	int tm;
	if (PyFloat_Check(arg0))
	{
		tm = (int)(1000 * PyFloat_AsDouble(arg0));
	}
	else if (PyLong_Check(arg0))
	{
		tm = (int)(1000 * PyLong_AsLong(arg0));
	}
	else
	{
		ASYNCFLOW_ERR("parse argument failed!\n");
		Py_RETURN_FALSE;
	}

	manager->Wait(tm);
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::set_var(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		Py_RETURN_FALSE;

	int varid;

	bool weak = false;
	int arg_size = PyTuple_Size(args);
	if(arg_size < 2)
	{
		ASYNCFLOW_ERR("set var need at least 2 arguments!\n");
		Py_RETURN_FALSE;
	}

	PyObject* key = PyTuple_GetItem(args, 0);
	PyObject* obj = PyTuple_GetItem(args, 1);

	if(arg_size == 3)
	{
		weak = PyTuple_GetItem(args, 2) == Py_True;
	}	

	if(PyLong_CheckExact(key))
	{
		varid = PyLong_AsLong(key);
	}
	else
	{
		auto name = PyUnicode_AsUTF8(key);
		varid = manager->GetCurrentNode()->GetChart()->GetData()->GetVarIndex(std::string(name));
	}
	const auto result = manager->SetVar(varid, obj, weak);
	return PyBool_FromLong(result);
}

PyObject* asyncflow::py::get_var(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		Py_RETURN_NONE;

	int varid;
	bool weak = false;
	auto* key = PyTuple_GetItem(args, 0);
	if(PyTuple_Size(args) > 0)
	{
		auto* py_weak = PyTuple_GetItem(args, 1);
		weak = py_weak == Py_True;
	}
	if (PyLong_CheckExact(key))
	{
		varid = PyLong_AsLong(key);
	}
	else
	{
		auto name = PyUnicode_AsUTF8(key);
		varid = manager->GetCurrentNode()->GetChart()->GetData()->GetVarIndex(std::string(name));
	}
	return manager->GetVar(varid, weak);
}

PyObject* asyncflow::py::get_event_param(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		Py_RETURN_NONE;

	int event_id;
	int param_id;
	if (!PyArg_ParseTuple(args, "ii", &event_id, &param_id))
	{
		ASYNCFLOW_ERR("parse argument failed!\n");
		Py_RETURN_NONE;
	}
	return manager->GetEventParam(event_id, param_id);
}

PyObject* asyncflow::py::wait_event(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		Py_RETURN_NONE;
	bool result = false;
	if (PyTuple_GET_SIZE(args) == 2)
	{
		PyObject* obj;
		int event_id;
		if (!PyArg_ParseTuple(args, "Oi", &obj, &event_id))
		{
			ASYNCFLOW_ERR("parse argument failed!\n");
			Py_RETURN_FALSE;
		}
		Agent* agent = manager->GetAgent(obj);
		if (agent == nullptr)
		{
			ASYNCFLOW_ERR("wait event obj {} is not registered", (void*)obj);
			agent = manager->RegisterGameObject(obj, Manager::DEFAULT_AGENT_TICK);
		}
		result = manager->WaitEvent(agent, event_id);
	}
	return PyBool_FromLong(result);
}

PyObject* asyncflow::py::call_sub(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		Py_RETURN_NONE;

	const char* chart_name = PyUnicode_AsUTF8(PyTuple_GetItem(args, 0));
	PyObject* obj = PyTuple_GetItem(args, 1);

	int size = (int)PyTuple_GET_SIZE(args);
	PyObject** argsArray = new PyObject * [size - 2];
	for (int i = 0; i < size - 2; i++)
		argsArray[i] = PyTuple_GetItem(args, i + 2);

	manager->Subchart(chart_name, obj, argsArray, size - 2);
	delete[] argsArray;
	return PyBool_FromLong(1);
}

PyObject* asyncflow::py::ret(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		Py_RETURN_NONE;

	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj))
	{
		ASYNCFLOW_ERR("parse argument failed!\n");
		Py_RETURN_FALSE;
	}

	const auto result = manager->Return(obj);
	return PyBool_FromLong(result);
}

PyObject* asyncflow::py::time(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		Py_RETURN_NONE;

	return PyLong_FromLongLong(manager->Now());
}


PyObject* asyncflow::py::wait_all(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	const auto argc = PyTuple_Size(args);
	std::vector<int> ids(argc);
	for (int i = 0; i < argc; i++)
	{
		ids[i] = PyLong_AsLong(PyTuple_GetItem(args, i));
	}
	manager->WaitAll(ids);
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::stop_node(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	const auto argc = PyTuple_Size(args);
	std::vector<int> ids(argc);
	for (int i = 0; i < argc; i++)
	{
		ids[i] = PyLong_AsLong(PyTuple_GetItem(args, i));
	}
	manager->StopNode(ids);
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::stop_flow(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	const auto argc = PyTuple_Size(args);
	std::vector<int> ids(argc);
	for (int i = 0; i < argc; i++)
	{
		ids[i] = PyLong_AsLong(PyTuple_GetItem(args, i));
	}
	manager->StopFlow(ids);
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::suspend_node(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		Py_RETURN_NONE;
	auto node_address = manager->CreateAsyncContext();
	return AsyncObject::New(node_address);	
}

PyObject* asyncflow::py::func(PyObject* self, PyObject* args)
{
	PyObject* func;
	if (!PyArg_ParseTuple(args, "O", &func))
	{
		ASYNCFLOW_ERR("asyncflow.func function failed!\n");
		Py_RETURN_NONE;
	}
	if (!PyFunction_Check(func))
	{
		ASYNCFLOW_ERR("asyncflow.func function failed!\n");
		Py_RETURN_NONE;
	}

	auto func_name = PyUnicode_AsUTF8(((PyFunctionObject*)func)->func_name);

	PyObject* async_module = self;
	if (CheckPythonException() || async_module == nullptr)
	{
		ASYNCFLOW_ERR("import asyncflow error in asyncflow.func");
		Py_RETURN_NONE;
	}
	PyObject* func_dict = PyObject_GetAttrString(async_module, "node_funcs");
	if (CheckPythonException() || func_dict == nullptr)
	{
		ASYNCFLOW_ERR("get node_funcs dict error in asyncflow.func");
		Py_RETURN_NONE;
	}
	Py_XINCREF(func);
	PyDict_SetItemString(func_dict, func_name, func);
	Py_RETURN_NONE;
}
#pragma endregion asyncflow_inner_func


#define ADD_PYTHON_FUNC(func)	{	#func,			asyncflow::py::func,				METH_VARARGS,	""}
static PyMethodDef asyncflow_python_module_methods[] =
{
	ADD_PYTHON_FUNC(setup),
	ADD_PYTHON_FUNC(exit),
	ADD_PYTHON_FUNC(import_charts),
	ADD_PYTHON_FUNC(patch),
	ADD_PYTHON_FUNC(import_event),
	{	"register",			(PyCFunction)register_obj,			METH_VARARGS,	""},
	ADD_PYTHON_FUNC(get_agent),
	{	"deregister",		(PyCFunction)deregister_obj,		METH_VARARGS,	""},
	ADD_PYTHON_FUNC(get_current_manager),
	ADD_PYTHON_FUNC(step),
    ADD_PYTHON_FUNC(attach),
	ADD_PYTHON_FUNC(remove),    
	ADD_PYTHON_FUNC(start),
	ADD_PYTHON_FUNC(stop),
	ADD_PYTHON_FUNC(event),
	ADD_PYTHON_FUNC(trigger),
	ADD_PYTHON_FUNC(config_log),
	ADD_PYTHON_FUNC(set_logger),
	ADD_PYTHON_FUNC(set_node_func),
	ADD_PYTHON_FUNC(get_charts),
	ADD_PYTHON_FUNC(wait),
	ADD_PYTHON_FUNC(stop_node),
	ADD_PYTHON_FUNC(stop_flow),	
	ADD_PYTHON_FUNC(suspend_node),
	ADD_PYTHON_FUNC(set_var),
	ADD_PYTHON_FUNC(get_var),
	ADD_PYTHON_FUNC(get_event_param),
	ADD_PYTHON_FUNC(wait_event),
	ADD_PYTHON_FUNC(ret),
	ADD_PYTHON_FUNC(wait_all),
	ADD_PYTHON_FUNC(call_sub),
	ADD_PYTHON_FUNC(func),
	ADD_PYTHON_FUNC(time),
	{	nullptr,			nullptr,			0,				nullptr }
};
#undef ADD_PYTHON_FUNC

#ifdef USING_PYTHON2
#define PyInit_asyncflow initasyncflow
#else
static struct PyModuleDef asyncflow_module = {
	PyModuleDef_HEAD_INIT,
	"asyncflow",
	nullptr,
	-1,
	asyncflow_python_module_methods
};
#endif

static PyObject* asyncflow_python_error;
PyMODINIT_FUNC PyInit_asyncflow(void)
{
	asyncflow::util::Log::Init();
#ifdef USING_PYTHON2
	PyObject* m = Py_InitModule("asyncflow", asyncflow_python_module_methods);
#else
	PyObject* m = PyModule_Create(&asyncflow_module);
	if (m == nullptr)
		return nullptr;
#endif


	
	InitEventIdObject(m);
	InitCustomPyObj(m);
	PyModule_AddObject(m, "node_funcs", PyDict_New());  //node_funcs table in asyncflow to store node functions

	asyncflow_python_error = PyErr_NewException("asyncflow.error", nullptr, nullptr);
	Py_INCREF(asyncflow_python_error);
	PyModule_AddObject(m, "error", asyncflow_python_error);
    _module = m;

#ifndef USING_PYTHON2
	return m;
#endif
}
