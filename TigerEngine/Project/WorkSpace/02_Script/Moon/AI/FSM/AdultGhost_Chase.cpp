#include "AdultGhost_Chase.h"
#include "EngineSystem/SceneSystem.h"

void AdultGhost_Chase::Enter()
{
    cout << "[AdultGhost_Chase] Enter Chase State" << endl;

    // 3초 동안은 무조건 플레이어 위치를 향해서 이동하고, (그리드에서 걸을 수 있는 영역만을 통해서 최단거리 이동)
    // 3초 이후에도 여전히 플레이어가 시야 범위 내에 있으면 여전히 플레이어 향해서 이동하지만, 
    // 3초 이후에 플레이어가 범위 내에 없으면 탐색Search 상태로 넘어간다. 

    agent = adultGhost->agent;
    player = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Player");

    chaseTimer = 0.0f;

    agent->patrolSpeed = 2.0f; // 추격 속도
    agent->isWaiting = false;
    agent->hasTarget = false;
    agent->path.clear();
}

void AdultGhost_Chase::ChangeStateLogic()
{
    if (!player) return;

    bool seen = adultGhost->vision->CheckVision(player, 90, 400);

    // 3초 이후에만 상태 전환 허용
    if (chaseTimer >= forceChaseTime && !seen)
    {
        adultGhost->ChangeState(AdultGhostState::Search);
    }
}

void AdultGhost_Chase::Update(float deltaTime)
{
    chaseTimer += deltaTime;

    if (!player) return;

    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;

    auto* tr = player->GetTransform();
    int px, py;
    grid->WorldToGridFromCenter(tr->GetWorldPosition(), px, py);

    // 플레이어 위치를 계속 목표로 갱신
    agent->targetCX = px;
    agent->targetCY = py;
    agent->hasTarget = true;
    agent->path.clear(); // 새 최단 경로 강제 생성 !! 
}

void AdultGhost_Chase::FixedUpdate(float deltaTime)
{

}

void AdultGhost_Chase::Exit()
{
    agent->hasTarget = false;
    agent->path.clear();
    agent->isWaiting = false;

    cout << "[AdultGhost_Chase] Exit" << endl;
}