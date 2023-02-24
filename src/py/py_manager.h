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
		class PyObjectRefHelper
		{
		public:
			static void DecRef(PyObject* obj) { Py_XDECREF(obj); }
			static void IncRef(PyObject* obj) { Py_XINCREF(obj); }
			static PyObject* Default() { return Py_None; }
		};

		class PyAgent;

		using AsyncEvent = core::AsyncEvent<PyObject*, PyObjectRefHelper>;

		class PyManager : public core::Manager
		{
		public:
			PyManager()
				: Manager()
				, agent_manager_(this) {}

			core::Chart* CreateChart() override
			{
				return new PyChart(this);
			}

			core::ChartData* CreateChartData() override
			{
				return new PyChartData();
			}

			core::AsyncEventBase* CreateAsyncEvent(int event_id, core::Agent* agent, void* args, int arg_count) override
			{
				auto event = new AsyncEvent(event_id, agent);
				event->SetArgs((PyObject**)args, arg_count);
				return event;
			}

			Agent* RegisterGameObject(PyObject* obj, int tick_interval)
			{
				if (tick_interval <= 0)
					tick_interval = DEFAULT_AGENT_TICK;
				if (agent_manager_.GetAgent(obj) != nullptr)
				{
					ASYNCFLOW_LOG("object has registered to asyncflow");
					return nullptr;
				}
				auto agent = agent_manager_.Register(obj);
				if (tick_interval != Manager::default_time_interval_)
					agent->SetTickInterval(tick_interval);
				return agent;
			}

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

			bool Subchart(std::string chart_name, PyObject* obj, PyObject** args, int arg_count)
			{
				auto* agent = agent_manager_.GetAgent(obj);
				//If the obj does not register then create a new one, its tick_interval as default
				if (agent == nullptr)
				{
					ASYNCFLOW_ERR("subchart object {0} is not registered", (       void*)obj);
					agent = agent_manager_.Register(obj);
				}

				return Manager::Subchart(chart_name, agent, args, arg_count);
			}

			bool StartAgent(PyObject* obj)
			{
				auto* agent = agent_manager_.GetAgent(obj);
				if (agent == nullptr)
					return false;

				agent->Start();
				return true;
			}

			core::Chart* AttachChart(PyObject* obj, const std::string& chart_name)
			{
				auto* agent = agent_manager_.Register(obj);
				return Manager::AttachChart(agent, chart_name);
			}

			core::AgentManager<PyAgent> agent_manager_;

			bool AsyncCallback(long long context, PyObject* v);		// 异步调用结束时继续往下运行
		};
	}
}