#include "export_class.h"
#include "export_python.h"
#include "py_common.h"
#include "py_manager.h"

using namespace asyncflow::py;

PyTypeObject ManagerObject::TypeObject = { PyObject_HEAD_INIT(NULL) };

PyMethodDef ManagerObject::methods_define[] = {
	{"register", (PyCFunction)register_obj, METH_VARARGS, "agent = mgr.register(gameobject)"},
	{"import_charts", (PyCFunction)import_charts, METH_VARARGS, "count = mgr.import_charts(chart_yaml)"},
	{"import_event", (PyCFunction)import_event, METH_VARARGS, "count = mgr.import_event(event_yaml)"},
	{"step", (PyCFunction)step, METH_VARARGS, "mgr.step(time_interval)"},
	{"event", (PyCFunction)event, METH_VARARGS, "mgr.event(gameobject, eventid, params...)"},
	{"deregister", (PyCFunction)deregister, METH_VARARGS, "mgr.deregister(gameobject)"},
	{"get_agent", (PyCFunction)get_agent, METH_VARARGS, "agent = mgr.get_agent()"},
	{nullptr}  // Sentinel
};

PyObject* ManagerObject::New(PyManager* ptr)
{
	auto* object = PyObject_New(ManagerObject, &TypeObject);
	object->ptr = ptr;
	return (PyObject*)object;
}

PyObject* ManagerObject::register_obj(TSELF* self, PyObject* args)
{
	PyObject* obj;
	int time_interval = Manager::DEFAULT_AGENT_TICK;
	if (!PyArg_ParseTuple(args, "O|i", &obj, &time_interval))
		PY_ARG_ERR;
	auto* mgr = self->ptr;
	if(mgr == nullptr)
	{
		ASYNCFLOW_ERR("manager has destroyed");
		Py_RETURN_NONE;
	}

	auto* agent = dynamic_cast<PyAgent*>(mgr->RegisterGameObject(obj, time_interval));
	if (agent == nullptr)
	{
		ASYNCFLOW_ERR("register failed or object has registered in asyncflow.register");
		Py_RETURN_NONE;
	}
	
	return AgentObject::New(agent);
}

PyObject* ManagerObject::deregister(TSELF* self, PyObject* args)
{
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj))
		PY_ARG_ERR;
	auto* mgr = self->ptr;
	const auto result = mgr->UnregisterGameObject(obj);
	return PyBool_FromLong(result);
}

PyObject* ManagerObject::import_charts(TSELF* self, PyObject* args)
{
	char* path;
	if (!PyArg_ParseTuple(args, "s", &path))
		PY_ARG_ERR;
	auto mgr = self->ptr;
	const auto result = mgr->ImportFile(path);
	return PyLong_FromLong(result);
}

PyObject* ManagerObject::import_event(TSELF* self, PyObject* args)
{
	char* path;
	if (!PyArg_ParseTuple(args, "s", &path))
		PY_ARG_ERR;

	auto mgr = self->ptr;
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

PyObject* ManagerObject::step(TSELF* self, PyObject* args)
{
	int i;
	if (!PyArg_ParseTuple(args, "i", &i))
		PY_ARG_ERR;

	auto mgr = self->ptr;
	mgr->Step(i);
	Py_RETURN_NONE;
}

PyObject* ManagerObject::event(TSELF* self, PyObject* args)
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
	auto mgr = self->ptr;
	mgr->Event(id, obj, event_args, args_count - 2);
	Py_RETURN_TRUE;
}

PyObject* ManagerObject::get_agent(TSELF* self, PyObject* args)
{
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj))
		PY_ARG_ERR;
	auto mgr = self->ptr;
	auto* agent = dynamic_cast<PyAgent*>(mgr->agent_manager_.GetAgent(obj));
	if (agent == nullptr)
	{
		ASYNCFLOW_ERR("This obj has not registered in asyncflow");
		Py_RETURN_NONE;
	}
	
	return AgentObject::New(agent);
}