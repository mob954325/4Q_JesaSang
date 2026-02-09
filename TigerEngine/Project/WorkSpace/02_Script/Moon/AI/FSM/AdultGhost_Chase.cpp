#include "AdultGhost_Chase.h"
#include "AdultGhost_Search.h"

#include "EngineSystem/SceneSystem.h"


void AdultGhost_Chase::Enter()
{
    cout << "[AdultGhost_Chase] Enter Chase State" << endl;

    chaseTimer = 0.0f;
    repathTimer = 0.0f;

    // Agent 초기화 
    adultGhost->ResetAgentForMove(4.0f); // Chase 속도

    // 타겟이 지정되어 있으면 그대로 사용
    if (!adultGhost->target)
    {
        adultGhost->target = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("AITarget");
    }
}

void AdultGhost_Chase::ChangeStateLogic()
{
    if (!adultGhost->target) return;

    // 이미 Attack으로 바뀌었으면 로직 중단
    if (adultGhost->state == AdultGhostState::Attack)
        return;

    // 최소 추격 시간 이후에만 추격 포기 가능함 
    if (CanGiveUpChase())
    {
        // 마지막으로 본 위치 저장 
        auto grid = GridSystem::Instance().GetMainGrid();
        if (grid)
        {
            int px, py;
            auto wp = adultGhost->target->GetTransform()->GetLocalPosition();

            // std::cout << "[Chase] Last Player World Pos = " << wp.x << ", " << wp.y << ", " << wp.z << std::endl;

            if (grid->WorldToGridFromCenter(wp, px, py))
            {
                std::cout << "[Chase] Save Last Player Grid = (" << px << ", " << py << ")" << std::endl;
                adultGhost->lastPlayerGrid = { px, py, true };
            }
            else
            {
                std::cout << "[Chase] WorldToGrid FAILED\n";
            }
        }
        // adultGhost->searchReason = SearchReason::FromChase;
        if (adultGhost->chaseReason == ChaseReason::FromBabyCry)
        {
            adultGhost->searchReason = SearchReason::None; // 필요 없거나 BabyCry용 처리
        }
        else
        {
            adultGhost->searchReason = SearchReason::FromChase;
        }
        adultGhost->ChangeState(AdultGhostState::Search);
    }
}

void AdultGhost_Chase::Update(float deltaTime)
{
    chaseTimer += deltaTime;
    repathTimer += deltaTime;

    if (!adultGhost->target) return;

    // 일정 주기로 목표 위치 갱신
    if (repathTimer >= repathInterval)
    {
        UpdateTargetGrid();
        repathTimer = 0.0f;
    }
}

void AdultGhost_Chase::FixedUpdate(float deltaTime)
{
    adultGhost->MoveToTarget(deltaTime);
}

void AdultGhost_Chase::Exit()
{
    adultGhost->agent->hasTarget = false;
    adultGhost->agent->externalControl = false;
    adultGhost->agent->path.clear();
}


// ------------------------------------------------------------------------- 

void AdultGhost_Chase::UpdateTargetGrid()
{
    auto grid = GridSystem::Instance().GetMainGrid();
    if (!grid) return;

    // [ 플레이어 위치 -> Grid ]
    int px, py;
    auto wp = adultGhost->target->GetTransform()->GetLocalPosition();
    if (!grid->WorldToGridFromCenter(wp, px, py)) return;
    // cout << "[Chase Repath] Player Grid = (" << px << "," << py << ")\n";

    // [ AI 위치 -> Grid ] : 디버그용
    //auto myPos = adultGhost->GetOwner()->GetTransform()->GetWorldPosition();
    //int cx, cy;
    //grid->WorldToGridFromCenter(myPos, cx, cy);
    //cout << "[Agent] Self Grid = (" << cx << "," << cy << ")\n";


    // [ 어느정도 이동했을 때 경로 갱신 ]
    int dist = abs(px - adultGhost->agent->targetCX)+ abs(py - adultGhost->agent->targetCY);

    const int repathThreshold = 2; // 2칸 이상 차이날 때만
    if (dist >= repathThreshold)
    {
        adultGhost->agent->targetCX = px;
        adultGhost->agent->targetCY = py;

        // 진행중인 경로가 거의 끝났을 때만 리셋
        if (adultGhost->agent->path.size() <= 2)
            adultGhost->agent->path.clear();

        adultGhost->agent->hasTarget = true;
    }
}

bool AdultGhost_Chase::CanGiveUpChase() const
{
    if (chaseTimer < minChaseTime)
        return false;

    //if (adultGhost->chaseReason == ChaseReason::FromBabyCry)
    //{
    //    return false; // BabyCry target이면 달래기 끝날때까지 포기 금지
    //}

    return !adultGhost->IsSeeing(adultGhost->target);
}