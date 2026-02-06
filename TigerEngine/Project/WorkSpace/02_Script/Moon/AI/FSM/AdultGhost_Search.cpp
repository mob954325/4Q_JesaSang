#include "AdultGhost_Search.h"
#include "Components/VisionComponent.h"
#include "EngineSystem/SceneSystem.h"

void AdultGhost_Search::Enter()
{
    cout << "[AdultGhost_Search] Enter Search State" << endl;

    // 초기화 
    agent = adultGhost->agent;
    agent->patrolSpeed = 5.0f;

    searchTimer = 0.0f;

    // (if) 기척 or 함정 감지로 넘어온 경우
    // - 3초 정지 하고 → 소리난 곳으로 이동 → 3~5초간 회전하며 대기

    // (if) 추격에서 추격 실패로 넘어온 경우
    // - 마지막으로 플레이어 목격 했던 지점으로 이동 후 → 3~5초간 회전하며 대기
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