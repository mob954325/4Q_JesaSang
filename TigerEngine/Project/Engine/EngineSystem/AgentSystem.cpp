#include "AgentSystem.h"
#include "../Components/AgentComponent.h"

void AgentSystem::Register(AgentComponent* a) 
{ 
    pending_agents.push_back(a);
}
void AgentSystem::UnRegister(AgentComponent* a)
{
    agents.erase(std::remove(agents.begin(), agents.end(), a), agents.end());
    pending_agents.erase(std::remove(pending_agents.begin(), pending_agents.end(), a), pending_agents.end());
}

void AgentSystem::FixedUpdate(float dt)
{
    for (auto& a : agents)
    {
        pending_agents.push_back(a);
    }
    pending_agents.clear();

    for (auto a : agents)
        a->OnFixedUpdate(dt);
}