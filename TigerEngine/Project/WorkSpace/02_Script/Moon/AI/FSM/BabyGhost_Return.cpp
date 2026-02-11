#include "BabyGhost_Return.h"

void BabyGhost_Return::Enter()
{
    cout << "[BabyGhost_Return] Enter Return State" << endl;

    babyGhost->ResetAgentForMove(3.0f); // Return 속도 

    visionTimer = 0.0f;

    babyGhost->animController->ChangeState("Idle");

    // 웨이포인트 좌표
    auto grid = GridSystem::Instance().GetMainGrid();
    if (grid)
    {
        int wx, wy;
        auto wp = babyGhost->initialPosition; // 웨이 포인트 = AI가 처음 배치된 위치
        if (grid->WorldToGridFromCenter(wp, wx, wy))
        {
            babyGhost->agent->targetCX = wx;
            babyGhost->agent->targetCY = wy;
            babyGhost->agent->hasTarget = true;
            babyGhost->agent->path.clear();
        }
    }
}

void BabyGhost_Return::ChangeStateLogic()
{ 
    // 3초 후 부터 시야에 플레이어 감지 (바로 감지하는거 방지하기 위해)
    if (visionTimer >= visionDelay)
    {
        auto* player = babyGhost->GetAITarget();
        if (player && babyGhost->IsSeeing(player))
        {
            cout << "[BabyGhost_Return] Player detected -> Cry" << endl;
            babyGhost->ChangeState(BabyGhostState::Cry);
            return;
        }
    }

    // 복귀 완료 : 현재 위치와 initialPosition 비교
    auto tr = babyGhost->GetOwner()->GetTransform();
    float distSqr = (tr->GetWorldPosition() - babyGhost->initialPosition).LengthSquared();
    const float arrivalThreshold = 150.0f; // 거의 도착했으면
    if (distSqr <= arrivalThreshold * arrivalThreshold)
    {
        cout << "[BabyGhost_Return] Reached waypoint -> Patrol" << endl;
        babyGhost->ChangeState(BabyGhostState::Patrol);
        return;
    }
}

void BabyGhost_Return::Update(float deltaTime)
{
}

void BabyGhost_Return::FixedUpdate(float deltaTime)
{
    babyGhost->MoveToTarget(deltaTime);
}

void BabyGhost_Return::Exit()
{
    babyGhost->agent->externalControl = false;
    babyGhost->agent->path.clear();
    babyGhost->agent->hasTarget = false;
}