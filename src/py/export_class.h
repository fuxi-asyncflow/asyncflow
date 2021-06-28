#pragma once
#ifdef USING_PYTHON
#include "Python.h"
#include "py_chart.h"

namespace asyncflow
{
	namespace py
	{
		typedef struct {
			PyObject_HEAD
				long long data_ptr;
		} BasicObject;

		void BasicObject_dealloc(BasicObject* self);

		void InitCustomPyObj(PyObject*);
		PyObject* CreateCustomPyObj(const std::string& cls_name, void* data_ptr);

		PyObject* mgr_register_obj(BasicObject* self, PyObject* args);
		PyObject* mgr_import_charts(BasicObject* self, PyObject* args);
		PyObject* mgr_import_event(BasicObject* self, PyObject* args);
		PyObject* mgr_step(BasicObject* self, PyObject* args);
		PyObject* mgr_event(BasicObject* self, PyObject* args);
		PyObject* mgr_deregister_obj(BasicObject* self, PyObject* args);
		PyObject* mgr_get_agent(BasicObject* self, PyObject* args);
		void InitManagerType();

		PyObject* agent_attach(BasicObject* self, PyObject* args);
		PyObject* agent_remove(BasicObject* self, PyObject* args);
		PyObject* agent_get_charts(BasicObject* self, PyObject* args);
		PyObject* agent_start(BasicObject* self, PyObject* args);
		PyObject* agent_stop(BasicObject* self, PyObject* args);
		PyObject* agent_get_obj(BasicObject* self, PyObject* args);
		PyObject* agent_get_chart(BasicObject* self, PyObject* args);
		void InitAgentType();

		PyObject* chart_set_callback(BasicObject* self, PyObject* args);
		void InitChartType();
	}
}

#endif