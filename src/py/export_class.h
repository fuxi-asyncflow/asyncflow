#pragma once
#ifdef USING_PYTHON
#include "Python.h"

namespace asyncflow
{
	namespace py
	{
		class PyManager;
		class PyAgent;
		class PyChart;

		template< typename T >
		struct CustomObject
		{
			PyObject_HEAD
		    T* ptr;

			using TSELF = CustomObject<T>;

			inline bool valid() { return ptr != nullptr; }
		};

		struct ManagerObject : CustomObject<PyManager>
		{
			static PyObject* New(PyManager*);
			static PyObject* is_valid(TSELF* self, PyObject* args);
			static PyMethodDef methods_define[];
			static PyTypeObject TypeObject;

			static PyObject* exit(TSELF* self, PyObject* args);
			static PyObject* register_obj(TSELF* self, PyObject* args);
			static PyObject* import_charts(TSELF* self, PyObject* args);
			static PyObject* import_event(TSELF* self, PyObject* args);
			static PyObject* step(TSELF* self, PyObject* args);
			static PyObject* event(TSELF* self, PyObject* args);
			static PyObject* deregister(TSELF* self, PyObject* args);
			static PyObject* get_agent(TSELF* self, PyObject* args);
		};

		struct AgentObject : CustomObject<PyAgent>
		{
			static PyObject* New(PyAgent*);
			static PyObject* is_valid(TSELF* self, PyObject* args);
			static PyMethodDef methods_define[];
			static PyTypeObject TypeObject;

			static PyObject* attach(TSELF* self, PyObject* args);
			static PyObject* remove(TSELF* self, PyObject* args);			
			static PyObject* start(TSELF* self, PyObject* args);
			static PyObject* stop(TSELF* self, PyObject* args);
			static PyObject* get_obj(TSELF* self, PyObject* args);
			static PyObject* get_chart(TSELF* self, PyObject* args);
			static PyObject* get_charts(TSELF* self, PyObject* args);
		};

		struct ChartObject : CustomObject<PyChart>
		{
			static PyObject* New(PyChart*);
			static PyObject* is_valid(TSELF* self, PyObject* args);
			static PyMethodDef methods_define[];
			static PyTypeObject TypeObject;

			static PyObject* set_callback(TSELF* self, PyObject* args);
			static PyObject* is_running(TSELF* self, PyObject*);
			static PyObject* start(TSELF* self, PyObject*);
			static PyObject* stop(TSELF* self, PyObject*);
		};

		void BasicObject_dealloc(PyObject* self);

		void InitCustomType(const char* name, PyTypeObject* tp, PyMemberDef* member, PyMethodDef* method);
		void InitCustomPyObj(PyObject*);
	}
}

#endif