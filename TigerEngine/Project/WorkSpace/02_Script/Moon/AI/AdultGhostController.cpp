#include "AdultGhostController.h"
#include "Object/GameObject.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

REGISTER_COMPONENT(AdultGhostController)

RTTR_REGISTRATION
{
    rttr::registration::class_<AdultGhostController>("AdultGhostController")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

nlohmann::json AdultGhostController::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void AdultGhostController::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void AdultGhostController::OnStart()
{
    agent = GetOwner()->GetComponent<AgentComponent>();
    if (!agent) return;

    agent->PickRandomTarget();
}

void AdultGhostController::OnFixedUpdate(float dt)
{
    if (!agent || !agent->hasTarget) { std::cout << "[AdultGhostController] OnFixedUpdate is NULL" << std::endl; return; }

    auto grid = GridSystem::Instance().GetMainGrid();
    Vector3 targetPos = grid->GridToWorldFromCenter(agent->targetCX, agent->targetCY);

    Vector3 pos = GetOwner()->GetTransform()->GetWorldPosition();
    Vector3 dir = targetPos - pos;

    if (dir.Length() < reachDist)
    {
        agent->cx = agent->targetCX;
        agent->cy = agent->targetCY;
        agent->hasTarget = false;
        agent->PickRandomTarget();
        return;
    }

    dir.y = 0;
    dir.Normalize();

    std::cout << "[AdultGhostController] OnFixedUpdate MoveAgent" << std::endl;
    agent->MoveAgent(dir, patrolSpeed, dt);
}