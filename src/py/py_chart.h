#pragma once
#include "core/chart.h"
#include <Python.h>

namespace asyncflow
{
	namespace py
	{
		class PyChart : public core::Chart
		{
		public:
			PyChart();
			~PyChart();
			bool	SetVar(int id, PyObject* v);
			PyObject*	GetVar(int id);
			void	SetCall(PyObject* call);
			void	SetInitTable(PyObject* tb);
			void SetArgs(void* args, int argc) override;
			void SetArgsFromDict(const std::map<std::string, PyObject*>& dict);
			void Return(PyObject* obj);
			void Return(bool result) override;
			void ClearVariables() override;
			void ResetVariables() override;
			bool InitArgs() override;

#ifdef FLOWCHART_DEBUG
			void SendEventStatus(std::string node_uid, const AsyncEventBase* event) override;
			void SendVariableStatus(std::string node_id, PyObject* old_value, PyObject* new_value);
#endif
			PyObject**		variables_;
			PyObject*		call_;		//	call as charts returning or no node to run
			PyObject*		init_table_; //	record the init variables table

		private:
			void InvokeCallback(PyObject* obj);
		};
	}
}