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
		};

		struct ManagerObject : CustomObject<PyManager>
		{
			static PyObject* New(PyManager*);

			static PyObject* register_obj(TSELF* self, PyObject* args);
			static PyObject* import_charts(TSELF* self, PyObject* args);
			static PyObject* import_event(TSELF* self, PyObject* args);
			static PyObject* step(TSELF* self, PyObject* args);
			static PyObject* event(TSELF* self, PyObject* args);
			static PyObject* deregister(TSELF* self, PyObject* args);
			static PyObject* get_agent(TSELF* self, PyObject* args);

			static PyMethodDef methods_define[];
			static PyTypeObject TypeObject;
		};

		struct AgentObject : CustomObject<PyAgent>
		{
			static PyObject* New(PyAgent*);

			static PyObject* attach(TSELF* self, PyObject* args);
			static PyObject* remove(TSELF* self, PyObject* args);
			static PyObject* get_charts(TSELF* self, PyObject* args);
			static PyObject* start(TSELF* self, PyObject* args);
			static PyObject* stop(TSELF* self, PyObject* args);
			static PyObject* get_obj(TSELF* self, PyObject* args);
			static PyObject* get_chart(TSELF* self, PyObject* args);

			static PyMethodDef methods_define[];
			static PyTypeObject TypeObject;
		};

		struct ChartObject : CustomObject<PyChart>
		{
			static PyObject* New(PyChart*);

			static PyObject* set_callback(TSELF* self, PyObject* args);

			static PyMethodDef methods_define[];
			static PyTypeObject TypeObject;
		    
		};

		void BasicObject_dealloc(PyObject* self);

		void InitCustomType(const char* name, PyTypeObject* tp, PyMemberDef* member, PyMethodDef* method);
		void InitCustomPyObj(PyObject*);
	}
}

#endif