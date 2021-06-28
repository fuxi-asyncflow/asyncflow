#pragma once

#include "core/node_func.h"
#include <Python.h>

namespace asyncflow
{
	namespace py
	{
		class PyNodeFunc : public core::NodeFunc
		{
		public:
			~PyNodeFunc();
			bool call(core::Agent* agent) override;
			static PyObject* CompileFunction(const std::string& code, const std::string& name);
			static NodeFunc* GetFuncFromString(const std::string& code, const std::string& name);
		private:
			int func_id_;
			std::string code_;
			PyObject* func_;
		};
	}
}