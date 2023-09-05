#pragma once
#include "core/agent.h"
#include <unordered_map>

namespace asyncflow
{
	namespace core
	{
		class Manager;

		class BasicAgentManager
		{
		public:
			BasicAgentManager(Manager* manager)
				: manager_(manager)
				, AGENT_ID_COUNT(0)
			{
				
			}

			BasicAgentManager(BasicAgentManager&) = delete;
			BasicAgentManager(BasicAgentManager&&) = delete;

			virtual ~BasicAgentManager()
			{
				Clear();
			}

			//The lua_agent should deconstruct before the lua_manager because of the refs, such as ObjectRef; Only use in lua_manager deconstructer
			void Clear()
			{
				for (const auto kv : agent_map_)
				{
					delete kv.second;
				}
				agent_map_.clear();
			}
			
			Agent* GetAgentById(Agent::AgentID id) const
			{
				auto const it = agent_map_.find(id);
				return it == agent_map_.end() ? nullptr : it->second;
			}

			void AddAgent(Agent* agent)
			{
				const auto id = GenAgentId();
				agent->SetId(id);
				agent_map_[id] =  agent;
			}

			const std::unordered_map<Agent::AgentID, Agent*>& GetAgents()
			{
				return agent_map_;
			}

			Agent::AgentID GenAgentId() { return ++AGENT_ID_COUNT; }

			virtual Agent* UnRegister(Agent* agent) = 0;
			
		protected:
			std::unordered_map<Agent::AgentID, Agent*> agent_map_;
			Manager* manager_;

			Agent::AgentID AGENT_ID_COUNT;
		};

		template<typename TAGENT>
		class AgentManager : public BasicAgentManager
		{			
		public:
			typedef typename TAGENT::TOBJ T;
			AgentManager(Manager* manager)
				: BasicAgentManager(manager)
			{
				
			}

			AgentManager(AgentManager&) = delete;
			AgentManager(AgentManager&&) = delete;

			~AgentManager() override
			{
				gameobject_map_.clear();
			}
			
			Agent* Register(T game_object)
			{
				auto* agent = GetAgent(game_object);
				if (agent != nullptr)
					return agent;
				ASYNCFLOW_LOG("register game object {}!", (void*)game_object);
				agent = new TAGENT(manager_, game_object);
				gameobject_map_[game_object] = agent;
				AddAgent(agent);
				return agent;
			}

			Agent* UnRegister(Agent* agent) override
			{
				if (agent == nullptr)
					return nullptr;

				agent_map_.erase(agent->GetId());
				auto* agt = dynamic_cast<TAGENT*>(agent);
				if(agt != nullptr)
				{
					gameobject_map_.erase(agt->GetGameObject());
				}				
				return agent;
			}

			Agent* GetAgent(T game_object)
			{
				auto const it = gameobject_map_.find(game_object);
				return it == gameobject_map_.end() ? nullptr : it->second;
			}

		private:			
			std::unordered_map<T, Agent*> gameobject_map_;			
		};
	}
}