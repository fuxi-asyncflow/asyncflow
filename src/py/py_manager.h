#pragma once

#include "core/manager.h"
#include "core/async_event.h"
#include "py_nodefunc.h"
#include "py_chartdata.h"
#include "py_agent.h"
#include "py_chart.h"
#include <Python.h>
#include "py_common.h"

namespace asyncflow
{
	namespace py
	{
		class PyAgent;

		using AsyncEvent = core::AsyncEvent<PyObject*, PyObjectRefHelper>;

		class PyManager : public core::Manager
		{
		public:
			PyManager();
			virtual ~PyManager();

			core::Chart* CreateChart() override	{ return new PyChart(this);	}
			core::ChartData* CreateChartData() override	{return new PyChartData();}
			core::AsyncEventBase* CreateAsyncEvent(int event_id, core::Agent* agent, void* args, int arg_count) override;

			Agent* RegisterGameObject(PyObject* obj, int tick_interval);
			bool UnregisterGameObject(PyObject* obj);

			bool Event(int event_id, PyObject* obj, PyObject** args, int arg_count, bool immediate = false);
			bool SetVar(int var_id, PyObject* obj);
			PyObject* GetVar(int var_id);
			PyObject* GetEventParam(int event_id, int param_idx);
			bool Return(PyObject*);
			PyObject* Invoke(const std::string& name, PyObject** args, int arg_count);
			PyAgent* GetAgent(PyObject* obj) { return (PyAgent*)agent_manager_.GetAgent(obj); }
			BasicAgentManager& GetAgentManager() override { return agent_manager_; }
			std::pair<bool, std::vector<std::string>> RunScript(const char* str) override;

			bool Subchart(std::string chart_name, PyObject* obj, PyObject** args, int arg_count);
			bool StartAgent(PyObject* obj);
		    core::Chart* AttachChart(PyObject* obj, const std::string& chart_name);

			bool AsyncCallback(long long context, PyObject* v);
			PyObject* GetExportObject() { PyObjectRefHelper::IncRef(export_object_); return export_object_; }

			static PyManager* GetCurrentManager() { return current_manager_; }
			static void SetCurrentManager(PyManager* mgr) { current_manager_ = mgr; }

		private:
			core::AgentManager<PyAgent> agent_manager_;
			PyObject* export_object_;
			static PyManager* current_manager_;
		};
	}
}