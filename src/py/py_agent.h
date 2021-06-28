#pragma once
#include "core/agent.h"
#include <Python.h>

namespace asyncflow
{
	namespace py
	{
		class PyAgent : public core::Agent
		{
		public:
			typedef PyObject* TOBJ;
			PyAgent(Manager* manager, PyObject* game_object);
			virtual		~PyAgent();
			PyObject*	GetRefObject() { return obj_; }
			TOBJ		GetGameObject() { return obj_; }
			std::string	GetName() override;

		private:
			PyObject*			obj_;						//game obj, self parameter in node function

		};
	}
}

