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
    AgentSystem::Instance().UnRegister(this);
}


void AgentComponent::OnInitialize()
{
    AgentSystem::Instance().Register(this);
}

void AgentComponent::OnStart()
{
    cct = GetOwner()->GetComponent<CharacterControllerComponent>();
    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;

    auto tr = GetOwner()->GetTransform();
    grid->WorldToGridFromCenter(tr->GetWorldPosition(), cx, cy);

    PickRandomTarget();
}

void AgentComponent::PickRandomTarget()
{
    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;

    int range = 10;

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
}

void AgentComponent::OnFixedUpdate(float dt)
{
    if (!cct || !hasTarget) { std::cout << "[AgentComponent] OnFixedUpdate is NULL" << std::endl; return; }

    auto grid = GridSystem::Instance().GetMainGrid();
    Vector3 targetPos = grid->GridToWorldFromCenter(targetCX, targetCY);

    Vector3 pos = GetOwner()->GetTransform()->GetWorldPosition();
    Vector3 dir = targetPos - pos;

    if (dir.Length() < reachDist)
    {
        cx = targetCX;
        cy = targetCY;
        hasTarget = false;
        PickRandomTarget();
        return;
    }

    dir.y = 0;
    dir.Normalize();

    // 여기서 CCT 이동
    std::cout << "[AgentComponent] OnFixedUpdate MoveAI" << std::endl;
    cct->MoveAI(dir, patrolSpeed, dt);
}
