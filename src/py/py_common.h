#pragma once
#include "Python.h"
#include<string>

static const char* manager_null_msg = "asyncflow func error , func `{0}` shoud called after asyncflow.setup()";
static const char* manager_null_msg_c = "asyncflow func error , func `%s` shoud called after asyncflow.setup()";
static const char* agr_err_msg = "asyncflow func arguments error in `{0}`";
static const char* agr_err_msg_c = "asyncflow func arguments error in `%s`";

#define PY_MGR_ERR												\
do {															\
    ASYNCFLOW_ERR(manager_null_msg, __func__);					\
	PyErr_Format(PyExc_RuntimeError, manager_null_msg_c, __func__);	\
    return nullptr;												\
} while (false)

#define PY_ARG_ERR												\
do {															\
    ASYNCFLOW_ERR(agr_err_msg, __func__);						\
	PyErr_Format(PyExc_RuntimeError, agr_err_msg_c, __func__);	\
    return nullptr;												\
} while (false)

#ifdef USING_PYTHON2
const char* PyUnicode_AsUTF8(PyObject * unicode);
const char* PyUnicode_AsUTF8AndSize(PyObject * unicode, Py_ssize_t * size);
#endif

namespace asyncflow
{
	namespace py
	{
		bool ObjIsBool(PyObject* obj);
		void FreeObject(PyObject* obj);
		bool CheckPythonException();
		std::string GetObjectName(PyObject* obj);		//get obj name in debug
		std::string ToString(PyObject* obj);    //convert obj to string in debug
        static PyObject* _module = nullptr;
        PyObject* GetModule();

		class PyObjectRefHelper
		{
		public:
			static void DecRef(PyObject* obj) { Py_XDECREF(obj); }
			static void IncRef(PyObject* obj) { Py_XINCREF(obj); }
			static PyObject* Default() { return Py_None; }
		};
	}
}

