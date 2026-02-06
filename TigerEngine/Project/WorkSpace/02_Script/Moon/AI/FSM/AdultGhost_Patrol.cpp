#include "AdultGhost_Patrol.h"
#include "Components/VisionComponent.h"
#include "EngineSystem/SceneSystem.h"

void AdultGhost_Patrol::Enter()
{
    cout << "[AdultGhost_Patrol] Enter Patrol State" << endl;

    agent = adultGhost->agent;

    // [ 순찰 : 목표 좌표 정하고 순찰 시작 ]
    agent->patrolSpeed = 1.2f;  // 순찰 속도 (상태마다 변경)
    agent->SetWaitTime(3.0f);   // 목표 지점에서 대기 시간 
    if (!agent->hasTarget && !agent->isWaiting)
        agent->PickRandomTarget();
}

void AdultGhost_Patrol::ChangeStateLogic()
{
    auto* player = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Player");

    if (player && adultGhost->vision->CheckVision(player, 90, 400))
    {
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