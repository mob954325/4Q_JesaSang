#include "AdultGhostController.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "Object/GameObject.h"
#include "EngineSystem/SceneSystem.h"

#include "FSM/IAdultGhostState.h"
#include "FSM/AdultGhost_Patrol.h"
#include "FSM/AdultGhost_Chase.h"


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

    // 기척 감지(순찰 → 탐색) : 시야 밖에서 기척(발소리 / 음식냄새) 감지
    // 함정 감지(순찰 → 탐색) : 시야 밖에서 함정 파동 감지

    // load animation
    //LoadAnimation();

    // init fsm
    InitFSMStates();
    ChangeState(AdultGhostState::Patrol);

    // init stat
    // InitStat();
}

void AdultGhostController::OnUpdate(float delta)
{
    // FSM 
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

    // AgentComponent의 경로 따라 이동 : TODO 수정 필요 
    agent->OnFixedUpdate(dt);  

    // FSM 
    if (curState)
    {
        curState->FixedUpdate(dt);
    }
}



// -----------------------------------------------------------
// [ FSM ]
// -----------------------------------------------------------

void AdultGhostController::InitFSMStates()
{
    //  Patrol, Chase, Search, Return, Attack, None
    fsmStates[(int)AdultGhostState::Patrol] = new AdultGhost_Patrol(this);
    fsmStates[(int)AdultGhostState::Chase] = new AdultGhost_Chase(this);
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

