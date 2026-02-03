#pragma once
#include "System/Singleton.h"
#include <vector>

class AgentComponent;

class AgentSystem : public Singleton<AgentSystem>
{
    std::vector<AgentComponent*> agents;

public:
    AgentSystem(token) {}

    void Register(AgentComponent* a);
    void UnRegister(AgentComponent* a);

    void FixedUpdate(float dt);
};
