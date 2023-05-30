#pragma once
#include "lua_common.h"
#include "core/agent.h"


namespace asyncflow
{
	namespace lua
	{
		class LuaAgent : public core::Agent
		{
		public:
			typedef void* TOBJ;
			LuaAgent(Manager* manager, void* game_object);
			virtual ~LuaAgent();			
			Ref		GetRefObject() { return obj_; }
			TOBJ		GetGameObject() { return addr_; }
			std::string GetName();
			bool		RunFlow(Node* start_node) override;

		private:
			Ref			obj_;						//game obj, self parameter in node function
			void*		addr_;
		};
	}
}

