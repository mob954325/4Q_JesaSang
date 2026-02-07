#include "AdultGhost_Search.h"
#include "EngineSystem/SceneSystem.h"


void AdultGhost_Search::Enter()
{
    cout << "[AdultGhost_Search] Enter Search State" << endl;

    searchTimer = 0.0f;
    arrived = false;

    adultGhost->ResetAgentForMove(3.5f);

    // 마지막 플레이어 위치 있으면 사용 
    if (adultGhost->lastPlayerGrid.valid)
    {
        auto& p = adultGhost->lastPlayerGrid;

        adultGhost->agent->targetCX = p.x;
        adultGhost->agent->targetCY = p.y;
        adultGhost->agent->hasTarget = true;

        adultGhost->lastPlayerGrid.valid = false; // 1회성
    }
    else
    {
        adultGhost->ChangeState(AdultGhostState::Patrol);  // 값 없으면 바로 Patrol
    }
}

void AdultGhost_Search::ChangeStateLogic()
{
    // 1. 플레이어 발견 (Search 성공)
    if (adultGhost->IsSeeing(adultGhost->GetAITarget()))
    {
        cout << "[AdultGhost_Search] Search Clear!! " << endl;
        adultGhost->ChangeState(AdultGhostState::Chase);
        return;
    }

    // 2. 도착 후 일정 시간 탐색 (Search 실패)
    if (arrived && searchTimer >= maxSearchTime)
    {
        cout << "[AdultGhost_Search] Search Fail.. " << endl;
        adultGhost->ChangeState(AdultGhostState::Patrol);
    }
}

void AdultGhost_Search::Update(float deltaTime)
{
    if (arrived) searchTimer += deltaTime;
}

void AdultGhost_Search::FixedUpdate(float deltaTime)
{
    if (!arrived)
    {
        bool done = adultGhost->MoveToTarget(deltaTime);
        if (done)
        {
            arrived = true;
            searchTimer = 0.0f;
        }
    }
}

void AdultGhost_Search::Exit()
{
    arrived = false;

    adultGhost->agent->externalControl = false;
    adultGhost->agent->path.clear();
    adultGhost->agent->hasTarget = false;
}