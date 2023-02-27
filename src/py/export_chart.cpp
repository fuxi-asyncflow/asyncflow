#include "export_class.h"
#include "export_python.h"
#include "py_common.h"
#include "py_agent.h"
#include "py_manager.h"
#include "py_chart.h"

using namespace asyncflow::py;

PyTypeObject ChartObject::TypeObject = { PyObject_HEAD_INIT(NULL) };

PyMethodDef ChartObject::methods_define[] = {
	{"set_callback", (PyCFunction)set_callback, METH_VARARGS, "set_callback"},
	{nullptr}
};

PyObject* ChartObject::New(PyChart* ptr)
{
	auto* object = PyObject_New(ChartObject, &TypeObject);
	object->ptr = ptr;
	return (PyObject*)object;
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