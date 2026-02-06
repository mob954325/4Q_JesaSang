#include "AdultGhost_Patrol.h"
#include "Components/VisionComponent.h"
#include "EngineSystem/SceneSystem.h"

void AdultGhost_Patrol::Enter()
{
    cout << "[AdultGhost_Patrol] Enter Patrol State" << endl;

    // 초기화 
    agent = adultGhost->agent;


    // [ 순찰 : 목표 좌표 정하고 순찰 시작 ]
    agent->patrolSpeed = 0.8f;  // 속도 
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
    // [ 기척 감지 ] 
    // 시야 밖에서 기척 감지 -> GetCurSenseRadiuse 로 확인 
    // 플레이어와 귀신 사이의 거리가 GetCurSenseRadiuse 보다 짧으면 => 기척 감지 

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