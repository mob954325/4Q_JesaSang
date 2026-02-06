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

}