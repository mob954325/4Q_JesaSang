#include "AdultGhostController.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "Object/GameObject.h"
#include "EngineSystem/SceneSystem.h"

#include "FSM/IAdultGhostState.h"
#include "FSM/AdultGhost_Patrol.h"
#include "FSM/AdultGhost_Chase.h"
#include "FSM/AdultGhost_Search.h"
#include "../../Woo/Object/HideObject.h"


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
}

void AdultGhostController::OnFixedUpdate(float dt)
{
    // FSM 
    if (curState)
    {
        curState->FixedUpdate(dt);
    }
}

void AdultGhostController::OnDestory()
{
    // HideObject를 보고 있다가 AI가 삭제 되었을 때
    if (hideLookRegistered && curSeeingHideObject)
    {
        auto* hideComp = curSeeingHideObject->GetComponent<HideObject>();
        if (hideComp)
        {
            hideComp->UnregisterAILook(this);
        }
    }
    hideLookRegistered = false;
    curSeeingHideObject = nullptr;
}


// -----------------------------------------------------------
// [ FSM ]
// -----------------------------------------------------------

void AdultGhostController::InitFSMStates()
{
    //  Patrol, Chase, Search, Return, Attack, None
    fsmStates[(int)AdultGhostState::Patrol] = new AdultGhost_Patrol(this);
    fsmStates[(int)AdultGhostState::Chase] = new AdultGhost_Chase(this);
    fsmStates[(int)AdultGhostState::Search] = new AdultGhost_Search(this);
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

}


// --------------------------------------------------------------------------


void AdultGhostController::OnPlayerNoise(const Vector3& noiseWorldPos)
{
    // Patrol 상태인 귀신만 반응
    if (state != AdultGhostState::Patrol)
        return;

    std::cout << "[AdultGhostController] OnPlayerNoise on Patrol AI" << std::endl;

    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;

    int cx, cy;
    if (!grid->WorldToGridFromCenter(noiseWorldPos, cx, cy))
        return;

    // Search 상태로 전환 + 목표 좌표 전달
    auto* search = dynamic_cast<AdultGhost_Search*>( fsmStates[(int)AdultGhostState::Search] );
    if (!search) return;

    search->SetSearchTarget(cx, cy);

    ChangeState(AdultGhostState::Search);
}
