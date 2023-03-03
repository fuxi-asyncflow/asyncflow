#include "export_class.h"
#include "export_python.h"
#include "py_common.h"
#include "py_agent.h"
#include "py_manager.h"
#include "py_chart.h"

using namespace asyncflow::py;

PyTypeObject ChartObject::TypeObject = { PyObject_HEAD_INIT(NULL) };

PyMethodDef ChartObject::methods_define[] = {
	{"is_valid", (PyCFunction)is_valid, METH_NOARGS, "return inside pointer to cpp object is valid or not"},
	{"set_callback", (PyCFunction)set_callback, METH_VARARGS, "set_callback"},
	{"is_running", (PyCFunction)is_running, METH_NOARGS, "if any node is running return true, else return false"},
	{"start", (PyCFunction)start, METH_NOARGS, "start chart, if chart is running, return false"},
	{"stop", (PyCFunction)stop, METH_NOARGS, "stop chart, if chart is not running, return false"},
	{nullptr}
};

PyObject* ChartObject::New(PyChart* ptr)
{
	auto* object = PyObject_New(ChartObject, &TypeObject);
	object->ptr = ptr;
	return (PyObject*)object;
}

PyObject* ChartObject::is_valid(TSELF* self, PyObject* args)
{
	return PyBool_FromLong(self->ptr != nullptr);
}

PyObject* ChartObject::set_callback(TSELF* self, PyObject* args)
{
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj) || !PyCallable_Check(obj))
		PY_ARG_ERR;
	auto* chart = self->ptr;
	chart->SetCall(obj);
	Py_RETURN_TRUE;
}

PyObject* ChartObject::is_running(TSELF* self, PyObject*)
{
	auto* chart = self->ptr;
	if (chart != nullptr && chart->GetStatus() == core::Chart::Status::Running)
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

PyObject* ChartObject::start(TSELF* self, PyObject*)
{
	auto* chart = self->ptr;	
	if (chart == nullptr || chart->GetStatus() == core::Chart::Status::Running)
		Py_RETURN_FALSE;
	return PyBool_FromLong(chart->GetAgent()->StartChart(chart));
}

PyObject* ChartObject::stop(TSELF* self, PyObject*)
{
	auto* chart = self->ptr;
	if (chart == nullptr || chart->GetStatus() != core::Chart::Status::Running)
		Py_RETURN_FALSE;
	chart->Stop();
	Py_RETURN_TRUE;
}