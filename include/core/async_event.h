#pragma once
#include <string>
#include "util/log.h"

namespace asyncflow
{
	namespace core
	{
		class Agent;	
		
		class AsyncEventBase
		{
		public:
			static int START_EVENT;
			static int TICK_EVENT;
			AsyncEventBase(int id, Agent* agent)
				: id_(id)
				, arg_count_(0)
				, agent_(agent)				
			{

			}
			virtual ~AsyncEventBase(){}

			Agent* GetAgent() const { return agent_; }
			int Id() const { return id_; }
			int argCount() const { return arg_count_; }

		protected:
			int		id_;
			int		arg_count_;
			Agent*	agent_;
		};

		template<typename T, typename RefHelper>
		class AsyncEvent : public AsyncEventBase
		{
		public:
			

			AsyncEvent(int id, Agent* agent)
				: AsyncEventBase(id, agent)
				, args_(nullptr)
			{

			}

			~AsyncEvent()
			{
				if (args_ != nullptr)
				{
					for (int i = 0; i < arg_count_; i++)
						helper.DecRef(args_[i]);
				}
				delete[] args_;
			}

			RefHelper helper;



			void  SetArgs(T* args, int argc)
			{
				args_ = args;
				arg_count_ = argc;
				for (int i = 0; i < argc; i++)
				{
					helper.IncRef(args_[i]);
				}
			}

			T GetEventParam(int idx) const
			{
				T result = helper.Default();
				if (idx >= arg_count_)
				{
					ASYNCFLOW_WARN("event param is out of range");
				}
				else
				{
					result = args_[idx];
				}
				helper.IncRef(result);		//TODO [ref]ref count+1?
				return result;
			}

		private:

			T*	args_;			
		};
	}
}