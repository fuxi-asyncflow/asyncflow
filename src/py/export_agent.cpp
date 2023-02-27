#include "export_class.h"
#include "export_python.h"
#include "py_common.h"
#include "py_agent.h"
#include "py_manager.h"
#include "py_chart.h"

using namespace asyncflow::py;

PyTypeObject AgentObject::TypeObject = { PyObject_HEAD_INIT(NULL) };

PyMethodDef AgentObject::methods_define[] = {
	{"attach", (PyCFunction)attach, METH_VARARGS, "agent_attach"},
	{"remove", (PyCFunction)remove, METH_VARARGS, "agent_remove"},
	{"get_charts", (PyCFunction)get_charts, METH_VARARGS, "agent_get_charts"},
	{"start", (PyCFunction)start, METH_NOARGS, "agent_start"	},
	{"stop", (PyCFunction)stop, METH_NOARGS, "agent_stop"},
	{"get_obj", (PyCFunction)get_obj, METH_NOARGS, "agent_get_obj"},
	{"get_chart", (PyCFunction)get_chart, METH_VARARGS, "agent_get_chart"},
	{nullptr}
};

PyObject* AgentObject::New(PyAgent* ptr)
{
	auto* object = PyObject_New(AgentObject, &TypeObject);
	object->ptr = ptr;
	return (PyObject*)object;
}

PyObject* AgentObject::attach(TSELF* self, PyObject* args)
{
	char* path;
	PyObject* params = Py_None;
	if (!PyArg_ParseTuple(args, "s|O", &path, &params))
		PY_ARG_ERR;
	auto agent = self->ptr;
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
	return ChartObject::New(chart);
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

PyObject* AgentObject::get_charts(TSELF* self, PyObject* args)
{
	auto* agent = self->ptr;
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
	auto obj = agent->GetRefObject();
	Py_INCREF(obj);
	return obj;
}

PyObject* AgentObject::get_chart(TSELF* self, PyObject* args)
{
	char* path;
	if (!PyArg_ParseTuple(args, "s", &path))
		PY_ARG_ERR;
	auto* agent = self->ptr;
	auto chart = (PyChart*)agent->FindChart(path, nullptr);
	if (chart == nullptr)
		Py_RETURN_NONE;
	return ChartObject::New(chart);
}