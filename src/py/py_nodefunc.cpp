#include "py_nodefunc.h"
#include "py_agent.h"
#include "py_manager.h"
#include "py_common.h"
#include "weakrefobject.h"

using namespace asyncflow::core;
using namespace asyncflow::py;

PyNodeFunc:: ~PyNodeFunc()
{
	Py_DECREF(func_);
}

NodeResult PyNodeFunc::call(Agent* agent)
{
	PyObject* self = ((PyAgent*)agent)->GetRefObject();
	auto py_result = PyObject_CallFunctionObjArgs((PyObject*)func_, self, nullptr);
	if (CheckPythonException())
	{
		//print error chart and variables in chart
		auto current_chart = (PyChart*)agent->GetManager()->GetCurrentNode()->GetChart();
		ASYNCFLOW_ERR("run node error in chart {0}", current_chart->Name());
		Chart* temp_chart = current_chart;
		while (temp_chart->GetOwnerNode() != nullptr)
		{
			ASYNCFLOW_ERR("The owner chart of {0} is {1}.", temp_chart->Name(), temp_chart->GetOwnerNode()->GetChart()->Name());
			temp_chart = temp_chart->GetOwnerNode()->GetChart();
		}
		for (int var_id = 0; var_id < current_chart->GetData()->GetVarCount(); var_id++)
		{
			auto pyobj = current_chart->GetVar(var_id);
			if (PyWeakref_CheckRef(pyobj))
				pyobj = PyWeakref_GetObject(pyobj);
			std::string var = ToString(pyobj);
			ASYNCFLOW_ERR("var_id is {0}, name is {1}, value is {2}.", var_id, current_chart->GetData()->GetVariableName(var_id), var);
			Py_XDECREF(pyobj);
		}
		return agent->GetManager()->IsNodeStopWhenError() ? rSTOP : rFALSE;
	}

	auto result = rTRUE;
    if (PyBool_Check(py_result))
        result = (py_result == Py_True) ? rTRUE : rFALSE;
    else
        result = PyObject_IsTrue(py_result) ? rTRUE : rFALSE;

	return result;
}

PyObject* PyNodeFunc::CompileFunction(const std::string& code, const std::string& name)
{
	PyObject *async_module = asyncflow::py::GetModule();
	if (CheckPythonException() || async_module == nullptr)
	{
		ASYNCFLOW_ERR("import asyncflow error in compile function");
		return nullptr;
	}
	PyObject *func_dict = PyObject_GetAttrString(async_module, "node_funcs");
	if (CheckPythonException() || func_dict == nullptr)
	{
		ASYNCFLOW_ERR("get node_funcs dict error");
		return nullptr;
	}
	PyFunctionObject* func = (PyFunctionObject*)PyDict_GetItemString(func_dict, name.c_str());
	Py_XINCREF(func);
	Py_DECREF(func_dict);
	if (!func)
	{
		ASYNCFLOW_ERR("cannot find func {0}, may name is wrong", name.c_str());
		return nullptr;
	}
	
	func->func_globals = PyEval_GetGlobals();
	//function collection causes the global module refcount to decrease
	Py_INCREF(func->func_globals);		
	return (PyObject*)func;
}


NodeFunc* PyNodeFunc::GetFuncFromString(const std::string& code, const std::string& name)
{
	auto func = CompileFunction(code, name);
	if (func == nullptr)
		return nullptr;
	auto f = new PyNodeFunc;
	f->func_ = func;
	Py_INCREF(func);
	return f;
}
