#include "export_class.h"
#include "py_common.h"
#include "structmember.h"
#include "py_agent.h"
#include "py_manager.h"
#include "export_python.h"
using namespace asyncflow::py;

void asyncflow::py::BasicObject_dealloc(PyObject* self)
{
	
}

void asyncflow::py::InitCustomType(const char* name, PyTypeObject* tp, PyMemberDef* members, PyMethodDef* methods)
{
	tp->tp_name = name;	
	tp->tp_basicsize = sizeof(CustomObject<void*>);
	tp->tp_members = members;
	tp->tp_methods = methods;
	tp->tp_dealloc = BasicObject_dealloc;

	if (PyType_Ready(tp) < 0)
		ASYNCFLOW_ERR("Export manager type fail");
	else
	{
		
		Py_INCREF(tp);
	}
}

void asyncflow::py::InitCustomPyObj(PyObject* m)
{
	InitCustomType("manager", &ManagerObject::TypeObject, nullptr, ManagerObject::methods_define);
	InitCustomType("agent", &AgentObject::TypeObject, nullptr, AgentObject::methods_define);
	InitCustomType("chart", &ChartObject::TypeObject, nullptr, ChartObject::methods_define);
}