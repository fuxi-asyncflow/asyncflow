#pragma once

namespace asyncflow
{
	namespace core
	{
		class Agent;
		class DyingAgents
		{
		public:
			DyingAgents() = default;

			std::vector<Agent*>& GetDyingAgents() { return destroying_agents_; }
			
			void AddDyingAgent(Agent* agent, bool in_step)
			{
				if (in_step)	
				//If the object is deregistered in the step, it is left for the next step to destroy
				{
					next_destroying_agents_.push_back(agent);
				}
				else
				//If the object is deregistered outside the step, then the next step destroy
				{
					destroying_agents_.push_back(agent);
				}
			}

			void Step()
			{
				destroying_agents_.clear();
				destroying_agents_.swap(next_destroying_agents_);
			}
			
		private:
			std::vector<Agent*> destroying_agents_;
			std::vector<Agent*> next_destroying_agents_;	//Store temporarily until the next step for destroying
		};
	}
}
