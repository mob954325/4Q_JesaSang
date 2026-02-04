#include "AdultGhostController.h"
#include "Object/GameObject.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "EngineSystem/SceneSystem.h"
#include "Components/VisionComponent.h"

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


// ------------------------------------------


void AdultGhostController::OnStart()
{
    agent = GetOwner()->GetComponent<AgentComponent>();
    vision = GetOwner()->GetComponent<VisionComponent>();

    if (!agent || !vision)
    {
        std::cout << "[AdultGhostController] agent or vision missing" << std::endl;
        return;
    }

    agent->PickRandomTarget(); // 목표 좌표 찾기 
}


void AdultGhostController::OnFixedUpdate(float dt)
{
    if (!agent || !vision) return;

    // AgentComponent의 경로 따라 이동
    agent->OnFixedUpdate(dt); 

    // 디버그
    //if (agent->hasTarget)
    //{
    //    std::cout << "[AdultGhostController] \"" << GetOwner()->GetName()
    //        << "\" Current: (" << agent->cx << "," << agent->cy << ") "
    //        << "Target: (" << agent->targetCX << "," << agent->targetCY << ") "
    //        << "Remaining Path: " << agent->path.size() << std::endl;
    //}


    // [시야 감지] 플레이어 탐지
    auto* player = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Player");
    if (player)
    {
        if (vision->CheckVision(player, 30, 400, targetMask, occlusionMask))
        {
            std::cout << "[AdultGhostController]" << GetOwner()->GetName() << " is PLAYER FOUND !" << std::endl;
        }
    }
}