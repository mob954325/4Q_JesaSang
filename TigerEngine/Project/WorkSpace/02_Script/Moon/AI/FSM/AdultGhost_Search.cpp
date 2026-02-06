#include "AdultGhost_Search.h"
#include "Components/VisionComponent.h"
#include "EngineSystem/SceneSystem.h"

void AdultGhost_Search::Enter()
{
    cout << "[AdultGhost_Search] Enter Search State" << endl;

    // 초기화 
    agent = adultGhost->agent;
    agent->patrolSpeed = 3.5f;

    agent->externalControl = true;
    agent->path.clear();
    agent->hasTarget = false;
    agent->isWaiting = false;

    arrived = false;
    searchTimer = 0.0f;

    if (hasSearchTarget)
    {
        agent->targetCX = targetCX;
        agent->targetCY = targetCY;
        agent->hasTarget = true;
    }

    // (if) 기척 or 함정 감지로 넘어온 경우
    // - 3초 정지 하고 → 소리난 곳으로 이동 → 3~5초간 회전하며 대기

    // (if) 추격에서 추격 실패로 넘어온 경우
    // - 마지막으로 플레이어 목격 했던 지점으로 이동 후 → 3~5초간 회전하며 대기
}

void AdultGhost_Search::ChangeStateLogic()
{
    // // 목적지에 도착하면 무조건 Patrol 상태로 전환
    // if (!arrived) return;

    // (임시) 무조건 5초 지나면 Patrol 상태로 전환 
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
    if (!agent || !agent->hasTarget) return;

    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;

    if (agent->path.empty())
    {
        agent->path = grid->FindPath(agent->cx, agent->cy, agent->targetCX, agent->targetCY);
        if (agent->path.empty()) return;
    }

    auto next = agent->path.front();
    Vector3 targetPos = grid->GridToWorldFromCenter(next.first, next.second);

    Vector3 pos = agent->GetOwner()->GetTransform()->GetWorldPosition();
    Vector3 dir = targetPos - pos;
    dir.y = 0;

    if (dir.Length() < agent->reachDist)
    {
        agent->cx = next.first;
        agent->cy = next.second;
        agent->path.erase(agent->path.begin());

        // 도착하면 타이머 시작
        if (agent->path.empty())
        {
            agent->hasTarget = false;
            arrived = true;
            searchTimer = 0.0f;
            cout << "[Search] Arrived at last position\n";
        }
    }
    else
    {
        dir.Normalize();
        agent->MoveAgent(dir, agent->patrolSpeed, deltaTime);
    }
}

void AdultGhost_Search::Exit()
{
    hasTarget = false;
    arrived = false;

    agent->externalControl = false; 
    agent->path.clear();
    agent->hasTarget = false;

    cout << "[AdultGhost_Search] Exit" << endl;
}