#include "export_class.h"
#include "export_python.h"
#include "py_common.h"
#include "py_agent.h"
#include "py_manager.h"
#include "py_chart.h"

using namespace asyncflow::py;

PyTypeObject AgentObject::TypeObject = { PyObject_HEAD_INIT(NULL) };

PyMethodDef AgentObject::methods_define[] = {
	{"is_valid", (PyCFunction)is_valid, METH_NOARGS, "return inside pointer to cpp object is valid or not"},
	{"attach", (PyCFunction)attach, METH_VARARGS, "agent_attach"},
	{"remove", (PyCFunction)remove, METH_VARARGS, "agent_remove"},
	{"get_graphs", (PyCFunction)get_charts, METH_NOARGS, "get all graphs attach on agent, include subgraph"},
	{"start", (PyCFunction)start, METH_NOARGS, "agent_start"	},
	{"stop", (PyCFunction)stop, METH_NOARGS, "agent_stop"},
	{"get_object", (PyCFunction)get_obj, METH_NOARGS, "agent_get_obj"},
	{"get_graph", (PyCFunction)get_chart, METH_VARARGS, "agent_get_chart"},
	{nullptr}
};

PyObject* AgentObject::New(PyAgent* ptr)
{
	auto* object = PyObject_New(AgentObject, &TypeObject);
	object->ptr = ptr;
	return (PyObject*)object;
}

PyObject* AgentObject::is_valid(TSELF* self, PyObject* args)
{
	return PyBool_FromLong(self->ptr != nullptr);
}

PyObject* AgentObject::attach(TSELF* self, PyObject* args)
{
	char* path;
	PyObject* params = Py_None;
	if (!PyArg_ParseTuple(args, "s|O", &path, &params))
		PY_ARG_ERR;
	auto* agent = self->ptr;
	auto* manager = dynamic_cast<PyManager*>(agent->GetManager());
	auto* chart = (PyChart*)(manager->Manager::AttachChart(agent, path));
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

PyObject* AgentObject::remove(TSELF* self, PyObject* args)
{
	char* chart_name;
	if (!PyArg_ParseTuple(args, "s", &chart_name))
		PY_ARG_ERR;

	auto* agent = self->ptr;
	const auto result = agent->RemoveChart(chart_name);
	return PyBool_FromLong(result);
}

PyObject* AgentObject::start(TSELF* self, PyObject* args)
{
	auto* agent = self->ptr;
	agent->Start();
	Py_RETURN_TRUE;
}

PyObject* AgentObject::stop(TSELF* self, PyObject* args)
{
	auto* agent = self->ptr;
	agent->Stop();
	Py_RETURN_TRUE;
}

PyObject* AgentObject::get_obj(TSELF* self, PyObject* args)
{
	auto* agent = self->ptr;
	if (agent == nullptr)
		Py_RETURN_NONE;
	auto* obj = agent->GetRefObject();
	Py_INCREF(obj);
	return obj;
}

PyObject* AgentObject::get_chart(TSELF* self, PyObject* args)
{
	auto* agent = self->ptr;
	if (agent == nullptr)
		Py_RETURN_NONE;
	char* path;
	if (!PyArg_ParseTuple(args, "s", &path))
		PY_ARG_ERR;
	
	auto chart = (PyChart*)agent->FindChart(path, nullptr);
	if (chart == nullptr)
		Py_RETURN_NONE;
	return chart->GetExportObject();
}

PyObject* AgentObject::get_charts(TSELF* self, PyObject* args)
{	
	auto* agent = self->ptr;
	if (agent == nullptr)
		Py_RETURN_NONE;
	auto& dict = agent->GetChartDict();
	std::vector<Chart*> v;

	for (auto& charts : dict)
	{
		v.insert(v.begin(), charts.second.begin(), charts.second.end());		
	}

	const auto size = v.size();
	auto* list = PyList_New(size);
	for(auto i = 0; i < size; i++)
	{
		PyList_SetItem(list, i, ((PyChart*)v[i])->GetExportObject());
	}	
	return list;
}