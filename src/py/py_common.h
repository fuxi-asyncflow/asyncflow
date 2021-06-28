#pragma once
#include "Python.h"
#include<string>

static const char* manager_null_msg = "manager is null, any function shoud called after asyncflow.setup()";
static const char* agr_err_msg = "parse argument failed!";

#define PY_MGR_ERR												\
do {															\
    ASYNCFLOW_ERR(manager_null_msg);							\
	PyErr_SetString(PyExc_RuntimeError, manager_null_msg);		\
    return nullptr;												\
} while (false)

#define PY_ARG_ERR												\
do {															\
    ASYNCFLOW_ERR(agr_err_msg);									\
	PyErr_SetString(PyExc_TypeError, agr_err_msg);				\
    return nullptr;												\
} while (false)

namespace asyncflow
{
	namespace py
	{
		bool ObjIsBool(PyObject* obj);
		void FreeObject(PyObject* obj);
		bool CheckPythonException();
		std::string GetObjectName(PyObject* obj);		//get obj name in debug
		std::string ToString(PyObject* obj);    //convert obj to string in debug
	}
}

