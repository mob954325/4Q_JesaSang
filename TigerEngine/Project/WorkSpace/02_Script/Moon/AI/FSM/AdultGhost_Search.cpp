#include "AdultGhost_Search.h"
#include "EngineSystem/SceneSystem.h"


void AdultGhost_Search::Enter()
{
    cout << "[AdultGhost_Search] Enter Search State" << endl;

    searchTimer = 0.0f;
    arrived = false;

    adultGhost->ResetAgentForMove(3.5f);

    TrySetInitialTarget();
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

    // 2. 목적지 도착못했는데, 시간 지났을 때 (Search 실패)
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
    hasSearchTarget = false;
    arrived = false;

    adultGhost->agent->externalControl = false;
    adultGhost->agent->path.clear();
    adultGhost->agent->hasTarget = false;
}

// -----------------------------------------------------------

void AdultGhost_Search::SetSearchTarget(int cx, int cy)
{
    targetCX = cx;
    targetCY = cy;
    hasSearchTarget = true;
}

void AdultGhost_Search::TrySetInitialTarget()
{
    if (!hasSearchTarget) return;

    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;

    adultGhost->agent->targetCX = targetCX;
    adultGhost->agent->targetCY = targetCY;
    adultGhost->agent->hasTarget = true;
}