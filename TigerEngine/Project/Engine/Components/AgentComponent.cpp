#include "AgentComponent.h"
#include "../Components/Transform.h"
#include "../Components/GridComponent.h"
#include "../Object/GameObject.h"
#include "../Util/JsonHelper.h"
#include "../EngineSystem/AgentSystem.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<AgentComponent>("AgentComponent")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)

        .property("reachDist", &AgentComponent::reachDist)
        .property("patrolSpeed", &AgentComponent::patrolSpeed);
}

nlohmann::json AgentComponent::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void AgentComponent::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

AgentComponent::~AgentComponent()
{
    
}


void AgentComponent::OnInitialize()
{
    
}

void AgentComponent::OnStart()
{
    cct = GetOwner()->GetComponent<CharacterControllerComponent>();

    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;

    auto tr = GetOwner()->GetTransform();
    grid->WorldToGridFromCenter(tr->GetWorldPosition(), cx, cy);
}

void AgentComponent::PickRandomTarget()
{
    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;

    int range = 10;

    hasTarget = false;

    for (int i = 0; i < 20; ++i)
    {
        int rx = cx + (rand() % (range * 2 + 1) - range);
        int ry = cy + (rand() % (range * 2 + 1) - range);

        if (grid->IsWalkableFromCenter(rx, ry))
        {
            targetCX = rx;
            targetCY = ry;
            hasTarget = true;
            return;
        }
    }

    std::cout << "[AgentComponent] No valid target found\n";
}

void AgentComponent::MoveAgent(const Vector3& dir, float speed, float dt)
{
    if (cct)
        cct->MoveAI(dir, speed, dt);
}

void AgentComponent::OnFixedUpdate(float dt)
{

}

void AgentComponent::Enable_Inner()
{
    AgentSystem::Instance().Register(this);
}

void AgentComponent::Disable_Inner()
{
    AgentSystem::Instance().UnRegister(this);
}
