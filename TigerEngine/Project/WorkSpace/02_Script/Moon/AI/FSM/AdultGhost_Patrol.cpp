#include "AdultGhost_Patrol.h"
#include "Components/VisionComponent.h"
#include "EngineSystem/SceneSystem.h"
#include "../../../Woo/Player/PlayerController.h"


void AdultGhost_Patrol::Enter()
{
    cout << "[AdultGhost_Patrol] Enter Patrol State" << endl;

    // 초기화 
    agent = adultGhost->agent;
    patrolTimer = 0.0f;

    // [ 순찰 : 목표 좌표 정하고 순찰 시작 ]
    agent->patrolSpeed = 0.8f;  // 속도 
    agent->SetWaitTime(3.0f);   // 목표 지점에서 대기 시간 

    if (!agent->hasTarget && !agent->isWaiting)
        agent->PickRandomTarget();
}

void AdultGhost_Patrol::ChangeStateLogic()
{
    // Patrol 들어오고 2초 동안은 감지 무시 (임시)
    if (patrolTimer < forcePatrolTime)
        return;


    // 1. 시야 감지 : 플레이어(AITarget) 감지 
    auto* AITarget = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("AITarget");
    if (AITarget && adultGhost->vision->CheckVision(AITarget, 90, 400))
    {
        cout << "[AdultGhost_Patrol] PLAYER FOUND ! " << endl;
        adultGhost->ChangeState(AdultGhostState::Chase);
        return;
    }

    // 2. 기척 감지 
    auto* playerObj = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Player");
    auto* playerController = playerObj->GetComponent<PlayerController>();
    if (!playerController) return;

    float senseRadius = playerController->GetCurSenseRadiuse();
    if (senseRadius <= 0.0f) return;

    Vector3 pPos = playerObj->GetTransform()->GetWorldPosition();
    Vector3 gPos = adultGhost->GetOwner()->GetTransform()->GetWorldPosition();

    float dist = Vector3::Distance(pPos, gPos); // = (pPos - gPos).Length();
    if (dist <= senseRadius)
    {
        cout << "[AdultGhost_Patrol] PLAYER FOUND (Sense)!  dist=" << dist << " radius=" << senseRadius << endl;

        adultGhost->ChangeState(AdultGhostState::Search);
    }
}

void AdultGhost_Patrol::Update(float deltaTime)
{
    patrolTimer += deltaTime;

    // [ 함정 오브젝트 연결 ]

    // [ 은신 오브젝트 연결 ]

}

void AdultGhost_Patrol::FixedUpdate(float deltaTime)
{
}

void AdultGhost_Patrol::Exit()
{
    agent->hasTarget = false;
    agent->path.clear();
    agent->isWaiting = false;

    cout << "[AdultGhost_Patrol] Exit" << endl;
}