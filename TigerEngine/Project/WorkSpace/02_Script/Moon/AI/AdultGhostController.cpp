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

    agent->PickRandomTarget(); // 목표 좌표 찾기 
}


void AdultGhostController::OnFixedUpdate(float dt)
{
    if (!agent)
    {
        std::cout << "[AdultGhostController] No agent attached!\n";
        return;
    }

    // AgentComponent의 경로 따라 이동 처리
    agent->OnFixedUpdate(dt);

    // 디버그 출력: 목표 좌표와 현재 위치 확인
    if (agent->hasTarget)
    {
        std::cout << "[DEBUG] Agent Current: (" << agent->cx << "," << agent->cy << ") "
            << "Target: (" << agent->targetCX << "," << agent->targetCY << ") "
            << "Remaining Path: " << agent->path.size() << std::endl;
    }
    else
    {
        std::cout << "[DEBUG] Agent has no target, picking new target next frame.\n";
    }
}
