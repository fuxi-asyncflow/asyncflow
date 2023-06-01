#include "export_class.h"
#include "export_python.h"
#include "py_common.h"
#include "py_manager.h"
#include <structmember.h>

using namespace asyncflow::py;

void AsyncObject::_dealloc(AsyncObject* self)
{
	Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* AsyncObject::New(int64_t handle)
{
	auto* type = &TypeObject;
	auto* self = reinterpret_cast<AsyncObject*>(type->tp_alloc(type, 0));	
    self->node_address = handle;	
	return reinterpret_cast<PyObject*>(self);
}

PyMethodDef AsyncObject::methods_define[] = {
	{"continue_node", (PyCFunction)Continue, METH_VARARGS, "active the async node and continue to run"},
	{"cancel", (PyCFunction)Cancel, METH_VARARGS, "cancel the async node"},
	{nullptr }
};

PyObject* AsyncObject::Continue(AsyncObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		Py_RETURN_FALSE;
	PyObject* obj;
	if (!PyArg_ParseTuple(args, "O", &obj))
	{
		ASYNCFLOW_ERR("parse argument failed!\n");
		Py_RETURN_FALSE;
	}
	
	bool result = manager->ContinueAsyncNode(self->node_address, obj);
	if (result)
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

PyObject* AsyncObject::Cancel(AsyncObject* self, PyObject* args)
{
	auto* manager = PyManager::GetCurrentManager();
	if (manager == nullptr)
		Py_RETURN_FALSE;
	PyObject* obj;
	int stop;
	if (!PyArg_ParseTuple(args, "p", &stop))
	{
		ASYNCFLOW_ERR("parse argument failed!\n");
		Py_RETURN_FALSE;
	}
	bool result = manager->CancelAsyncNode(self->node_address, stop);
	if (result)
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

PyTypeObject AsyncObject::TypeObject = {	PyObject_HEAD_INIT(nullptr) };

void AsyncObject::InitType(PyObject* module)
{	
	TypeObject.tp_name = "asyncflow.AsyncObject";
	TypeObject.tp_basicsize = sizeof(AsyncObject);
	TypeObject.tp_doc = "AsyncObject used for callback";
	TypeObject.tp_dealloc = (destructor)_dealloc;
	TypeObject.tp_methods = methods_define;

	if (PyType_Ready(&TypeObject) < 0)
		return;

	Py_INCREF(&TypeObject);
	PyModule_AddObject(module, "AsyncObject", (PyObject*)&TypeObject);
}