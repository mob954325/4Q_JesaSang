#include "AdultGhost_Search.h"
#include "EngineSystem/SceneSystem.h"


void AdultGhost_Search::Enter()
{
    cout << "[AdultGhost_Search] Enter Search State" << endl;

    arrived = false;
    waitTimer = 0.0f;
    rotateTimer = 0.0f;

    adultGhost->ResetAgentForMove(3.5f);
    adultGhost->animController->ChangeState("Idle");

    phase = SearchPhase::WaitBeforeMove;

    // 1. Chase or Attack 에서 넘어온 경우 바로 이동
    if (adultGhost->searchReason == SearchReason::FromChase ||
        adultGhost->searchReason == SearchReason::FromAttack)
    {
        phase = SearchPhase::MoveToPoint;
    }

    // 2. 마지막 플레이어 위치 있으면 사용 
    if (adultGhost->lastPlayerGrid.valid)
    {
        auto& p = adultGhost->lastPlayerGrid;
        // std::cout << "[Search] Use Last Grid = (" << p.x << ", " << p.y << ")" << std::endl;

        adultGhost->agent->targetCX = p.x;
        adultGhost->agent->targetCY = p.y;
        adultGhost->agent->hasTarget = true;

        adultGhost->lastPlayerGrid.valid = false; // 1회성
    }

    // 3) Patrol에서 넘어왔는데도 lastPlayerGrid가 없으면 바로 Return (혹은 RotateSearch로 할까?)
    if (phase == SearchPhase::WaitBeforeMove && !adultGhost->agent->hasTarget)
    {
        adultGhost->ChangeState(AdultGhostState::Return);
        return;
    }
}

void AdultGhost_Search::ChangeStateLogic()
{
    // 1. 회전 중 플레이어 발견 (Search 성공)
    if (phase == SearchPhase::RotateSearch &&  adultGhost->IsSeeing(adultGhost->GetAITarget()))
    {
        cout << "[AdultGhost_Search] Search Clear!! " << endl;
        adultGhost->ChangeState(AdultGhostState::Chase);
        return;
    }
    // 2. 회전 시간 종료 (Search 실패)
    if (phase == SearchPhase::RotateSearch && rotateTimer >= rotateTime)
    {
        cout << "[AdultGhost_Search] Search Fail.." << endl;
        adultGhost->ChangeState(AdultGhostState::Return);
    }
}

void AdultGhost_Search::Update(float deltaTime)
{
    if (phase == SearchPhase::WaitBeforeMove)
    {
        waitTimer += deltaTime;
        if (waitTimer >= waitTime)
        {
            phase = SearchPhase::MoveToPoint;
        }
    }
}

void AdultGhost_Search::FixedUpdate(float deltaTime)
{
    if (phase == SearchPhase::MoveToPoint && !arrived)
    {
        bool done = adultGhost->MoveToTarget(deltaTime);
        if (done)
        {
            arrived = true;
            phase = SearchPhase::RotateSearch;
            rotateTimer = 0.0f;

            // 도착 즉시 목표 정리
            adultGhost->agent->externalControl = true;
            adultGhost->agent->path.clear();
            adultGhost->agent->hasTarget = false;

            auto tr = adultGhost->GetOwner()->GetTransform();
            baseYaw = tr->GetYaw();
            targetYaw = baseYaw + XMConvertToRadians(360.f);

            adultGhost->agent->externalControl = true;
        }
    }
    else if (phase == SearchPhase::RotateSearch)
    {
        rotateTimer += deltaTime;

        auto tr = adultGhost->GetOwner()->GetTransform();
        float newYaw = tr->GetYaw() + XMConvertToRadians(90.f) * deltaTime;

        tr->SetEuler(Vector3(0.f, newYaw, 0.f));
    }
}

void AdultGhost_Search::Exit()
{
    arrived = false;

    adultGhost->agent->externalControl = false;
    adultGhost->agent->path.clear();
    adultGhost->agent->hasTarget = false;
}