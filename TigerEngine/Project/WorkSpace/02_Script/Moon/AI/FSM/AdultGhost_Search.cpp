#include "AdultGhost_Search.h"
#include "Components/VisionComponent.h"
#include "EngineSystem/SceneSystem.h"

void AdultGhost_Search::Enter()
{
    cout << "[AdultGhost_Search] Enter Search State" << endl;

    // 초기화 
    agent = adultGhost->agent;
    agent->patrolSpeed = 0.8f;

    searchTimer = 0.0f;
}

void AdultGhost_Search::ChangeStateLogic()
{
    // 3초 이후 무조건 Patrol 상태 전환 (임시) 
    if (searchTimer >= forceSearchTime)
    {
        cout << "[AdultGhost_Search] ChangeState -> AdultGhostState::Patrol" << endl;
        adultGhost->ChangeState(AdultGhostState::Patrol);
    }
}

void AdultGhost_Search::Update(float deltaTime)
{
    searchTimer += deltaTime;
}

void AdultGhost_Search::FixedUpdate(float deltaTime)
{
}

void AdultGhost_Search::Exit()
{
    cout << "[AdultGhost_Search] Exit" << endl;
}