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

bool _get_int_from_config_dict(PyObject* dict, const char* key, int& value)
{
	auto* result = PyDict_GetItemString(dict, key);
	if (result == nullptr)
	{
		return false;
	}

	if (PyLong_Check(result))
	{
		value = (int)PyLong_AsLong(result);
		return true;
	}
	else
	{
		ASYNCFLOW_WARN("{0} must be integer", key);
		return false;
	}
}

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
		auto result = PyDict_GetItemString(config, "defer_event");
		if (result != nullptr)
		{
			manager->SetDeferMode(PyObject_IsTrue(result));			
		}

		int step_time = 0;
		if(_get_int_from_config_dict(config, "default_timestep", step_time))
		{
			manager->SetDefaulTimeInterval(step_time);
		}

		result = PyDict_GetItemString(config, "node_stop_when_error");
		if (result != nullptr)
		{
			manager->SetNodeStopWhenError(PyObject_IsTrue(result));
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
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;
	return _event(self, args, !manager->isDeferMode());
}

PyObject* asyncflow::py::trigger(PyObject* self, PyObject* args)
{
	return _event(self, args, true);
}

PyObject* asyncflow::py::post(PyObject* self, PyObject* args)
{
	return _event(self, args, false);
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
		printf("parse argument failed in set_logger! first arg should be a callable object\n");
		Py_RETURN_FALSE;
	}

	auto asyncflow_logger = spdlog::get("asyncflow");
	if (asyncflow_logger == nullptr)
	{
		printf("[asyncflow] set_logger failed! you should call config_log with asyncflow as logger before set_logger");
		Py_RETURN_FALSE;
	}

	for (auto& sink : asyncflow_logger->sinks())
	{		
		auto python_sink = std::dynamic_pointer_cast<spdlog::python_logger_sink>(sink);
		if (python_sink)
		{
			python_sink->set(python_log);
			Py_RETURN_TRUE;
		}
	}

	auto py_sink = std::make_shared<spdlog::python_logger_sink>(python_log);
	asyncflow_logger->sinks().push_back(py_sink);
	Py_RETURN_TRUE;
}

PyObject* asyncflow::py::set_node_func(PyObject* self, PyObject* args)
{
	//PyObject* async_module = PyImport_ImportModule("asyncflow");
    PyObject* async_module = self;
	if (async_module == nullptr)
		async_module = asyncflow::py::_module;
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
#ifdef USING_PYTHON2
	else if(PyInt_Check(arg0))
	{
		tm = (int)(1000 * PyInt_AsLong(arg0));
	}
#endif
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
	if(PyTuple_Size(args) > 1)
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

		if (!Py_IsNone(obj))
		{
			Agent* agent = manager->GetAgent(obj);
			if (agent == nullptr)
			{
				ASYNCFLOW_ERR("wait event obj {} is not registered", (void*)obj);
				agent = manager->RegisterGameObject(obj, Manager::DEFAULT_AGENT_TICK);
			}
			result = manager->WaitEvent(agent, event_id);
		}
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

PyObject* asyncflow::py::dump(PyObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		PY_MGR_ERR;

	PyObject* obj = nullptr;
	if (PyTuple_Size(args) < 1)
	{
		ASYNCFLOW_ERR("dump function need at least 1 argument");
		Py_RETURN_NONE;
	}
	obj = PyTuple_GetItem(args, 0);

	PyObject* result = PyList_New(0);
	if (result == nullptr)
		Py_RETURN_NONE;

	auto* agent = manager->GetAgent(obj);
	if(agent == nullptr)
		Py_RETURN_NONE;

	auto& dict = agent->GetChartDict();
	std::vector<Chart*> v;

	for (auto& charts : dict)
	{
		v.insert(v.begin(), charts.second.begin(), charts.second.end());
	}

	for(auto* chart : v)
	{
		auto data = PyDict_New();
		const auto* chart_data = chart->GetData();
		const auto& path = chart_data->Name();
		PyDict_SetItemString(data, "path", PyUnicode_FromStringAndSize(path.c_str(), path.size()));

		// node_list
		const auto nodes_count = chart->GetNodesCount();
		auto* nodes_list = PyList_New(nodes_count);
		for(int i=0; i<nodes_count; i++)
		{
			const auto* node = chart->GetNode(i);
			if(node == nullptr)
			{
				PyList_SetItem(nodes_list, i, PyList_New(0));
				continue;
			}

			auto* node_info = PyList_New(5);
			auto* node_data = node->GetData();
			const auto& uid = node_data->GetUid();
			PyList_SetItem(node_info, 0, PyUnicode_FromStringAndSize(uid.c_str(), uid.size()));		// uid
			PyList_SetItem(node_info, 1, PyLong_FromLong(node_data->GetId()));			// id
			PyList_SetItem(node_info, 2, PyLong_FromLong(node->GetTrueCount()));		// true_count
			PyList_SetItem(node_info, 3, PyLong_FromLong(node->GetFalseCount()));		// false_count
			PyList_SetItem(node_info, 4, PyLong_FromLong(node->IsRunning()));			// is_running

			PyList_SetItem(nodes_list, i, node_info);
		}
		PyDict_SetItemString(data, "nodes", nodes_list);

		// variables
		const auto var_count = chart->GetData()->GetVarCount();
		auto* var_dict = PyDict_New();
		for(int i=0; i<var_count; i++)
		{
			const auto& var_name = chart_data->GetVariableName(i);
			auto* var_obj = ((PyChart*)chart)->GetVar(i, true);
			auto var_str = ToString(var_obj);
			PyDict_SetItemString(var_dict, var_name.c_str(), PyUnicode_FromStringAndSize(var_str.c_str(), var_str.size()));			
		}
		PyDict_SetItemString(data, "variables", var_dict);

		//stack
		auto* cur_node = chart->GetOwnerNode();
		int cur_stack = 1;
		const int MAX_STACK = 32;
		auto* stack_list = PyList_New(0);
		while(cur_stack < MAX_STACK && cur_node != nullptr)
		{
			auto cur_chart_path = cur_node->GetChart()->GetData()->Name();
			const auto& uid = cur_node->GetData()->GetUid();
			auto* stack_info = PyList_New(3);
			PyList_SetItem(stack_info, 0, PyUnicode_FromStringAndSize(cur_chart_path.c_str(), cur_chart_path.size()));
			PyList_SetItem(stack_info, 1, PyUnicode_FromStringAndSize(uid.c_str(), uid.size()));
			PyList_SetItem(stack_info, 2, PyLong_FromLong(cur_node->GetId()));
			PyList_Append(stack_list, stack_info);

            cur_stack++;
            cur_node = cur_node->GetChart()->GetOwnerNode();
		}

		PyDict_SetItemString(data, "stack", stack_list);

		PyList_Append(result, data);
	}
	return result;
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
	ADD_PYTHON_FUNC(post),
	ADD_PYTHON_FUNC(config_log),
	ADD_PYTHON_FUNC(set_logger),
	ADD_PYTHON_FUNC(set_node_func),
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
    ADD_PYTHON_FUNC(dump),
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
