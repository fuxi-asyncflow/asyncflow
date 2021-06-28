#pragma once
#include "core/node_func.h"

namespace asyncflow
{
	namespace js
	{
		class JsNodeFunc : public core::NodeFunc
		{
		public:
			bool call(core::Agent* agent) override;
			static NodeFunc* GetFuncFromString(const std::string& code, const std::string& name);

			std::string name;

		};
	}
}