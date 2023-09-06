#pragma once
#ifdef USING_PYTHON

#include "Python.h"

namespace asyncflow
{
	namespace py
	{
		
		PyObject* setup(PyObject* self, PyObject* args);
		PyObject* import_charts(PyObject* self, PyObject* args);
		PyObject* patch(PyObject* self, PyObject* args);
		PyObject* import_event(PyObject* self, PyObject* args);
		PyObject* register_obj(PyObject* self, PyObject* args);
		PyObject* get_current_manager(PyObject* self, PyObject* args);
		PyObject* get_agent(PyObject* self, PyObject* args);
		PyObject* step(PyObject* self, PyObject* args);
		PyObject* event(PyObject* self, PyObject* args);
		PyObject* trigger(PyObject* self, PyObject* args);
		PyObject* post(PyObject* self, PyObject* args);
		PyObject* _event(PyObject* self, PyObject* args, bool trigger);
		PyObject* deregister_obj(PyObject* self, PyObject* args);
		PyObject* exit(PyObject* self, PyObject* args);
		PyObject* attach(PyObject* self, PyObject* args);
		PyObject* remove(PyObject* self, PyObject* args);
		PyObject* start(PyObject* self, PyObject* args);
		PyObject* stop(PyObject* self, PyObject* args);
		PyObject* config_log(PyObject* self, PyObject* args);
		PyObject* set_logger(PyObject* self, PyObject* args);
		PyObject* set_node_func(PyObject* self, PyObject* args);

		PyObject* stop_node(PyObject* self, PyObject* args);
		PyObject* stop_flow(PyObject* self, PyObject* args);
		PyObject* wait(PyObject* self, PyObject* args);
		PyObject* set_var(PyObject* self, PyObject* args);
		PyObject* get_var(PyObject* self, PyObject* args);
		PyObject* get_event_param(PyObject* self, PyObject* args);
		PyObject* wait_event(PyObject* self, PyObject* args);
		PyObject* call_sub(PyObject* self, PyObject* args);
		PyObject* ret(PyObject* self, PyObject* args);
		PyObject* suspend_node(PyObject* self, PyObject* args);
		PyObject* wait_all(PyObject* self, PyObject* args);
		PyObject* time(PyObject* self, PyObject* args);
		PyObject* func(PyObject* self, PyObject* args);
		PyObject* dump(PyObject* self, PyObject* args);

		typedef struct {
			PyObject_HEAD
		}EventIdObject;
		extern PyTypeObject EventIdType;

		void InitEventIdObject(PyObject*);
	}
}





PyMODINIT_FUNC PyInit_asyncflow(void);


#endif
