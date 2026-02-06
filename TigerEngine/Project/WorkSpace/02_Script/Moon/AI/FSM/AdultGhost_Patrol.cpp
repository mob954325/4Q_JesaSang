#include "AdultGhost_Patrol.h"
#include "Components/VisionComponent.h"
#include "EngineSystem/SceneSystem.h"

void AdultGhost_Patrol::Enter()
{
    cout << "[AdultGhost_Patrol] Enter Patrol State" << endl;

    // 초기화 
    agent = adultGhost->agent;


    // [ 순찰 : 목표 좌표 정하고 순찰 시작 ]
    agent->patrolSpeed = 1.2f;  // 순찰 속도 (상태마다 변경)
    agent->SetWaitTime(3.0f);   // 목표 지점에서 대기 시간 

    if (!agent->hasTarget && !agent->isWaiting)
        agent->PickRandomTarget();
}

void AdultGhost_Patrol::ChangeStateLogic()
{
    // [ 시야 감지 : 플레이어 감지 ]
    auto* AITarget = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("AITarget");
    if (AITarget && adultGhost->vision->CheckVision(AITarget, 90, 400))
    {
        cout << "[AdultGhost_Patrol] PLAYER FOUND ! " << endl;
        adultGhost->ChangeState(AdultGhostState::Chase);
    }
}

void AdultGhost_Patrol::Update(float deltaTime)
{
}

void AdultGhost_Patrol::FixedUpdate(float deltaTime)
{
    
}

void AdultGhost_Patrol::Exit()
{
    auto* agent = adultGhost->agent;

    agent->hasTarget = false;
    agent->path.clear();
    agent->isWaiting = false;

    cout << "[AdultGhost_Patrol] Exit" << endl;
}