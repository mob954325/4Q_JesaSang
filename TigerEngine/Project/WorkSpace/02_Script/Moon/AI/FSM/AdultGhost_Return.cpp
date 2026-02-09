#include "AdultGhost_Return.h"

void AdultGhost_Return::Enter()
{
    cout << "[AdultGhost_Return] Enter Return State" << endl;

    adultGhost->ResetAgentForMove(3.0f); // Return 속도 

    // 웨이포인트 좌표
    auto grid = GridSystem::Instance().GetMainGrid();
    if (grid)
    {
        int wx, wy;
        auto wp = adultGhost->initialPosition; // 웨이 포인트 = AI가 처음 배치된 위치
        if (grid->WorldToGridFromCenter(wp, wx, wy))
        {
            adultGhost->agent->targetCX = wx;
            adultGhost->agent->targetCY = wy;
            adultGhost->agent->hasTarget = true;
            adultGhost->agent->path.clear();
        }
    }
}

void AdultGhost_Return::ChangeStateLogic()
{
    // 시야에 플레이어가 들어오면 Chase
    auto* player = adultGhost->GetAITarget();
    if (player && adultGhost->IsSeeing(player))
    {
        cout << "[AdultGhost_Return] Player detected -> Chase" << endl;
        adultGhost->ChangeState(AdultGhostState::Chase);
        return;
    }

    // 복귀 완료 : 현재 위치와 initialPosition 비교
    auto tr = adultGhost->GetOwner()->GetTransform();
    float distSqr = (tr->GetWorldPosition() - adultGhost->initialPosition).LengthSquared();
    const float arrivalThreshold = 150.0f; // 거의 도착했으면
    if (distSqr <= arrivalThreshold * arrivalThreshold)
    {
        cout << "[AdultGhost_Return] Reached waypoint -> Patrol" << endl;
        adultGhost->ChangeState(AdultGhostState::Patrol);
        return;
    }
}

void AdultGhost_Return::Update(float deltaTime)
{
}

void AdultGhost_Return::FixedUpdate(float deltaTime)
{
    adultGhost->MoveToTarget(deltaTime);
}

void AdultGhost_Return::Exit()
{
    adultGhost->agent->externalControl = false;
    adultGhost->agent->path.clear();
    adultGhost->agent->hasTarget = false;
}