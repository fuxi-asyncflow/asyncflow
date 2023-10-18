#include "py_common.h"
#include "util/log.h"
#include <frameobject.h>
#include "core/agent.h"

#include <sstream>

#if PY_VERSION_HEX < 0x030900B1
static inline PyCodeObject* PyFrame_GetCode(PyFrameObject* frame)
{
	Py_INCREF(frame->f_code);
	return frame->f_code;
}
#endif

std::string py_error_message;

const char* asyncflow::py::GetPythonErrorMessage(int* length)
{
	if (length != nullptr)
		*length = 0;
	if(py_error_message.empty())
		return nullptr;
	*length = py_error_message.length();
	return py_error_message.c_str();
}

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
		py_error_message.clear();
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
			const char* msg = PyUnicode_AsUTF8(pRepr);
			error_message = error_message + "- exception value:" + msg;
			py_error_message.assign(msg);
			Py_DecRef(pRepr);
			Py_DecRef(pExcValue);
		}
		if (pExcTraceback != nullptr)
		{
			int stack_num = 0;
			error_message = error_message + "- exception traceback: ";
			for (auto tb = (PyTracebackObject*)pExcTraceback; tb != nullptr; tb = tb->tb_next)
			{
				const auto* code = PyFrame_GetCode(tb->tb_frame);				
				const auto* file_name = PyUnicode_AsUTF8(code->co_filename);
				const auto* co_name = PyUnicode_AsUTF8(code->co_name);

				std::stringstream ss;
				ss << "[" << stack_num++ << "]:  File `" << file_name << "`, line " << tb->tb_lineno << ", in " << co_name << "; ";
				error_message += ss.str();
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
#ifdef USING_PYTHON2
		auto size = Agent::DEBUG_NAME_METHOD.size();
		std::vector<char> func_name( size + 1);
		auto* buffer = &func_name[0];
		memcpy(buffer, Agent::DEBUG_NAME_METHOD.c_str(), size);
		func_name[size] = 0;
		name_obj = PyObject_CallMethod(obj, buffer, nullptr);
#else
		name_obj = PyObject_CallMethod(obj, Agent::DEBUG_NAME_METHOD.c_str(), nullptr);
#endif
		
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

#ifdef USING_PYTHON2
//TODO  test and support utf8 string
const char* PyUnicode_AsUTF8(PyObject* unicode)
{
	Py_ssize_t size;
	return PyUnicode_AsUTF8AndSize(unicode, &size);
}

const char* PyUnicode_AsUTF8AndSize(PyObject* unicode, Py_ssize_t* size)
{
	char* buffer;
	PyString_AsStringAndSize(unicode, &buffer, size);
	return buffer;
}
#endif
