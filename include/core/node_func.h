#pragma once

#include "core/manager.h"
#include <string>
#include <vector>
#include "gsl/span"

using gsl::span;

namespace asyncflow
{
	namespace core
	{
		class Agent;		

		class NodeFunc
		{
		public:			
			virtual bool call(Agent* agent) = 0;
			virtual ~NodeFunc() {}
		};

		typedef bool(Manager::*FuncType)(span<const int>);

		class NodeInnerFunc : public NodeFunc
		{
		public:
			bool call(core::Agent* agent) override;
			static NodeFunc* CreateInnerFunc(FuncType, std::vector<int>);

		private:
			std::vector<int> params_;
			FuncType func;
		};

#ifdef FLOWCHART_DEBUG
		class BreakpointFunc : public NodeFunc
		{
		public:
			BreakpointFunc(NodeFunc* func):continue_flag_(false), pre_nodefunc_(func) {}

			bool call(core::Agent* agent) override;
			void SetExecute(bool flag) { continue_flag_ = flag; }
			NodeFunc* GetPreFunc() { return pre_nodefunc_; }

		private:
			NodeFunc* pre_nodefunc_;
			bool continue_flag_;
		};
#endif
	}
}