#pragma once
#include "core/chart.h"
#include "py_common.h"

namespace asyncflow
{
	namespace py
	{
		class PyChart : public core::Chart
		{
		public:
			PyChart(Manager* mgr);
			~PyChart();
			bool	SetVar(int id, PyObject* v, bool weak = false);
			PyObject*	GetVar(int id, bool weak = false);
			void	SetCall(PyObject* call);
			void	SetInitTable(PyObject* tb);
			void SetArgs(void* args, int argc) override;
			void SetArgsFromDict(const std::map<std::string, PyObject*>& dict);
			void Return(PyObject* obj);
			void Return(bool result) override;
			void ClearVariables() override;
			void ResetVariables() override;
			bool InitArgs() override;
			PyObject* GetExportObject() { PyObjectRefHelper::IncRef(export_object_); return export_object_; }

#ifdef FLOWCHART_DEBUG
			void SendEventStatus(std::string node_uid, const AsyncEventBase* event) override;
			void SendVariableStatus(std::string node_id, PyObject* old_value, PyObject* new_value);
#endif
			PyObject**		variables_;
			PyObject*		call_;		//	call as charts returning or no node to run
			PyObject*		init_table_; //	record the init variables table
			PyObject*		export_object_;

		private:
			void InvokeCallback(PyObject* obj);
		};
	}
}