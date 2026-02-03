#include "AgentSystem.h"
#include "../Components/AgentComponent.h"

void AgentSystem::Register(AgentComponent* a) 
{ 
    agents.push_back(a); 
}
void AgentSystem::UnRegister(AgentComponent* a)
{
    agents.erase(std::remove(agents.begin(), agents.end(), a), agents.end());
}

void AgentSystem::FixedUpdate(float dt)
{
    for (auto a : agents)
        a->OnFixedUpdate(dt);
}