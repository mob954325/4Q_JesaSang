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
#include "../../Woo/Player/PlayerController.h"


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

    //LoadAnimation();  

    InitFSMStates();
    ChangeState(AdultGhostState::Patrol);

}

void AdultGhostController::OnUpdate(float delta)
{
    if (currentState)
    {
        currentState->ChangeStateLogic();
        currentState->Update(delta);
    }
}

void AdultGhostController::OnFixedUpdate(float dt)
{
    if (currentState)
    {
        currentState->FixedUpdate(dt);
    }
}

void AdultGhostController::OnDestory()
{
    // AI가 HideObject를 보고 있다가 삭제 되었을 때
    if (hideLookRegistered && curSeeingHideObject)
    {
        if (auto* hide = curSeeingHideObject->GetComponent<HideObject>())
            hide->UnregisterAILook(this);
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
    if (currentState == fsmStates[(int)nextState])
        return;

    if (currentState)
        currentState->Exit();

    currentState = fsmStates[(int)nextState];
    this->state = nextState;

    if (currentState)
        currentState->Enter();
}


void AdultGhostController::LoadAnimation()
{

}



// -------------------------------------------------
// Movement
// -------------------------------------------------
bool AdultGhostController::MoveToTarget(float delta)
{
    if (!agent || !agent->hasTarget) return false;

    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return false;

    // 경로 없으면 생성
    if (agent->path.empty())
    {
        agent->path = grid->FindPath(agent->cx, agent->cy, agent->targetCX, agent->targetCY);
        if (agent->path.empty()) return false;
    }

    // 다음 칸 이동 
    auto next = agent->path.front();
    Vector3 targetPos = grid->GridToWorldFromCenter(next.first, next.second);

    Vector3 pos = agent->GetOwner()->GetTransform()->GetWorldPosition();
    Vector3 dir = targetPos - pos;
    dir.y = 0;

    // 해당 칸 도착 
    if (dir.Length() < agent->reachDist)
    {
        agent->cx = next.first;
        agent->cy = next.second;
        agent->path.erase(agent->path.begin());
        return agent->path.empty(); // 이 칸은 끝까지 가도록 
    }

    dir.Normalize();
    agent->MoveAgent(dir, agent->patrolSpeed, delta);
    return false;
}


// -------------------------------------------------
// Helper
// -------------------------------------------------

void AdultGhostController::ResetAgentForMove(float speed)
{
    agent->patrolSpeed = speed;
    agent->externalControl = true;
    agent->hasTarget = false;
    agent->path.clear();
    agent->isWaiting = false;
}

// Ai가 Target을 보고 있는가? // TODO : FOV, Dist 값 매개변수로 받기 
bool AdultGhostController::IsSeeing(GameObject* target) const
{
    return target && vision->CheckVision(target, 90, 400);
}

// Object Getter 
GameObject* AdultGhostController::GetAITarget() const
{
    return SceneSystem::Instance().GetCurrentScene() ->GetGameObjectByName("AITarget");
}
GameObject* AdultGhostController::GetPlayer() const
{
    return SceneSystem::Instance().GetCurrentScene() ->GetGameObjectByName("Player");
}

// Ai가 플레이어의 기척 범위에 들어왔는가? 
bool AdultGhostController::IsPlayerInSenseRange() 
{
    auto* playerObj = GetPlayer();
    if (!playerObj) return false;

    auto* playerController = playerObj->GetComponent<PlayerController>();
    if (!playerController) return false;

    float senseRadius = playerController->GetCurSenseRadiuse();
    if (senseRadius <= 0) return false;

    Vector3 pPos = playerObj->GetTransform()->GetWorldPosition();
    Vector3 gPos = this->GetOwner()->GetTransform()->GetWorldPosition();

    return Vector3::Distance(pPos, gPos) <= senseRadius;
}


// -------------------------------------------------
// Interaction
// -------------------------------------------------

// 플레이어에서 호출 
void AdultGhostController::OnPlayerNoise(const Vector3& noiseWorldPos)
{
    // Patrol 상태인 귀신만 반응
    if (state != AdultGhostState::Patrol) return;

    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;

    int cx, cy;
    if (!grid->WorldToGridFromCenter(noiseWorldPos, cx, cy))
        return;

    // Search 상태로 전환 + 목표 좌표 설정 
    lastPlayerGrid = { cx, cy, true };

    ChangeState(AdultGhostState::Search);
}
