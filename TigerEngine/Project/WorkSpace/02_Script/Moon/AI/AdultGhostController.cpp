#include "AdultGhostController.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "Object/GameObject.h"
#include "EngineSystem/SceneSystem.h"

#include "FSM/IAdultGhostState.h"


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
        std::cout << "[AdultGhostController] Component Missing" << std::endl;
        return;
    }

    // 목표 좌표 찾기 (이전 코드)
    // agent->PickRandomTarget(); 

    // load animation
    //LoadAnimation();

    // init fsm
    InitFSMStates();
    ChangeState(AdultGhostState::Patrol);

    // init stat
    InitStat();
}

void AdultGhostController::OnUpdate(float delta)
{
    // fsm
    if (curState)
    {
        curState->ChangeStateLogic();
        curState->Update(delta);
    }

    // interaction cheak
    // InteractionCheak(delta);
}


void AdultGhostController::OnFixedUpdate(float dt)
{
    if (!agent || !vision) return;

    // AgentComponent의 경로 따라 이동
    // agent->OnFixedUpdate(dt);  

#pragma region PathDebug 
    //if (agent->hasTarget)
    //{
    //    std::cout << "[AdultGhostController] \"" << GetOwner()->GetName()
    //        << "\" Current: (" << agent->cx << "," << agent->cy << ") "
    //        << "Target: (" << agent->targetCX << "," << agent->targetCY << ") "
    //        << "Remaining Path: " << agent->path.size() << std::endl;
    //}
#pragma endregion 


    // 시야 감지 : 플레이어 탐지
    auto* player = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Player");
    if (player)
    {
        if (vision->CheckVision(player, 90, 400))
        {
            std::cout << "[AdultGhostController]" << GetOwner()->GetName() << " is PLAYER FOUND !" << std::endl;
        }
    }

}

void AdultGhostController::InteractionCheak(float delta)
{
    // SerachObjectInteraction(delta);   
}