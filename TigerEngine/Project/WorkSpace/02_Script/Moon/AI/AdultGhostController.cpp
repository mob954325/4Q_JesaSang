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


// -----------------------------------------------------------
// [ Process ]
// -----------------------------------------------------------

void AdultGhostController::OnStart()
{
    agent = GetOwner()->GetComponent<AgentComponent>();
    vision = GetOwner()->GetComponent<VisionComponent>();

    if (!agent || !vision)
    {
        std::cout << "[AdultGhostController] Component Missing" << std::endl;
        return;
    }

    // 목표 좌표 찾기
    agent->SetWaitTime(3.0f); // 대기 시간 
    agent->PickRandomTarget(); 

    // load animation
    //LoadAnimation();

    // init fsm
    //InitFSMStates();
    //ChangeState(AdultGhostState::Patrol);

    // init stat
    // InitStat();
}

void AdultGhostController::OnUpdate(float delta)
{
    //// fsm
    //if (curState)
    //{
    //    curState->ChangeStateLogic();
    //    curState->Update(delta);
    //}

    // interaction cheak
    // InteractionCheak(delta);
}


void AdultGhostController::OnFixedUpdate(float dt)
{
    if (!agent || !vision) return;

    // AgentComponent의 경로 따라 이동
    agent->OnFixedUpdate(dt);  

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



// -----------------------------------------------------------
// [ FSM ]
// -----------------------------------------------------------

void AdultGhostController::InitFSMStates()
{
    //  Patrol, Chase, Search, Return, Attack, None
    // fsmStates[(int)AdultGhostState::Patrol] = new AdultGhost_Patrol(this);
    //fsmStates[(int)AdultGhostState::Chase] = new AdultGhost_Chase(this);
    //fsmStates[(int)AdultGhostState::Search] = new AdultGhost_Search(this);
    //fsmStates[(int)AdultGhostState::Return] = new AdultGhost_Return(this);
    //fsmStates[(int)AdultGhostState::Attack] = new AdultGhost_Attack(this);
}

void AdultGhostController::ChangeState(AdultGhostState nextState)
{
    if (curState == fsmStates[(int)nextState])
        return;

    if (curState)
        curState->Exit();

    curState = fsmStates[(int)nextState];
    this->state = nextState;

    if (curState)
        curState->Enter();
}

void AdultGhostController::LoadAnimation()
{
    //// 애니메이션 파일 로드
    //FBXResourceManager::Instance().LoadAnimationByPath(fbxData->GetFBXInfo(), "..\\Assets\\Resource\\Animation\\ani_walk_character.fbx", "Idle");

    //// 클립 생성
    //auto idleClip = animController->FindClip("Idle");

    //if (!idleClip)
    //{
    //    OutputDebugStringW(L"[CCTTest] Clip not found! 이름 확인 필요\n");
    //    return;
    //}

    //// 상태 등록
    //animController->AddState(std::make_unique<AnimationState>("Idle", idleClip, animController));

    //// 시작
    //animController->ChangeState("Idle");
}

