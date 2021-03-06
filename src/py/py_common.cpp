#include "py_common.h"
#include "util/log.h"
#include <frameobject.h>
#include "core/agent.h"

bool asyncflow::py::ObjIsBool(PyObject* obj)
{
	return PyObject_IsTrue(obj) == 1;
}

void asyncflow::py::FreeObject(PyObject* obj)
{
	Py_DecRef(obj);
}

bool asyncflow::py::CheckPythonException()
{
	if (PyErr_Occurred())
	{
		// get the error details
		std::string error_message = "An error occurred:";
		PyObject *pExcType, *pExcValue, *pExcTraceback;
		PyErr_Fetch(&pExcType, &pExcValue, &pExcTraceback);
		if (pExcType != nullptr)
		{
			PyObject* pRepr = PyObject_Repr(pExcType);
			error_message = error_message + "- exception type:" + PyUnicode_AsUTF8(pRepr);
			Py_DecRef(pRepr);
			Py_DecRef(pExcType);
		}
		if (pExcValue != nullptr)
		{
			PyObject* pRepr = PyObject_Repr(pExcValue);
			error_message = error_message + "- exception value:" + PyUnicode_AsUTF8(pRepr);
			Py_DecRef(pRepr);
			Py_DecRef(pExcValue);
		}
		if (pExcTraceback != nullptr)
		{
			for (auto tb = (PyTracebackObject*)pExcTraceback; tb != nullptr; tb = tb->tb_next)
			{
				PyObject *line = PyUnicode_FromFormat("  File \"%U\", line %d, in %U",
					tb->tb_frame->f_code->co_filename,
					tb->tb_lineno,
					tb->tb_frame->f_code->co_name);
				error_message = error_message + "- exception traceback:" + PyUnicode_AsUTF8(line);
				Py_XDECREF(line);
			}
		}

		PyErr_Clear();
		ASYNCFLOW_ERR("{0}", error_message.c_str());

		return true;
	}
	return false;
}

std::string asyncflow::py::GetObjectName(PyObject* obj)
{
	PyObject* name_obj = nullptr;
	if (PyObject_HasAttrString(obj, Agent::DEBUG_NAME_METHOD.c_str()))
	{
		name_obj = PyObject_CallMethod(obj, Agent::DEBUG_NAME_METHOD.c_str(), nullptr);
		if (CheckPythonException()) return "-";
	}

	if (name_obj == nullptr)
	{
		FreeObject(name_obj);
		name_obj = PyObject_Str(obj);
		if (CheckPythonException() || name_obj == nullptr)
			return "-";
	}

	Py_ssize_t length = 0;
	auto name = PyUnicode_AsUTF8AndSize(name_obj, &length);
	FreeObject(name_obj);
	return std::string(name, length);
}

std::string asyncflow::py::ToString(PyObject* obj)
{
	PyObject* obj_string = nullptr;
	obj_string = PyObject_Str(obj);

	if (CheckPythonException() || obj_string == nullptr)
	{
		return "-";
	}

	Py_ssize_t length = 0;
	auto str = PyUnicode_AsUTF8AndSize(obj_string, &length);
	FreeObject(obj_string);
	return std::string(str, length);
}
