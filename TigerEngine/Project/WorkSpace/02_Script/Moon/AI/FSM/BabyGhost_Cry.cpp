#include "BabyGhost_Cry.h"

#include "EngineSystem/SceneSystem.h"
#include "../AdultGhostController.h"


void BabyGhost_Cry::Enter()
{
    cout << "[BabyGhost_Cry] Enter Cry State" << endl;

    careTimer = 0.0f;

    // 이동 완전 정지
    babyGhost->agent->externalControl = true;
    babyGhost->agent->path.clear();

    // 현재 울음 위치
    auto cryPos = babyGhost->GetOwner()->GetTransform()->GetWorldPosition();

    // 주변 어른 유령 찾기
    auto adultGhosts = SceneSystem::Instance().GetCurrentScene()->GetGameObjectsByName("Ghost_Adult");

    AdultGhostController* closestPatrolGhost = nullptr;
    float minDist = FLT_MAX;

    for (auto* go : adultGhosts)
    {
        auto* adult = go->GetComponent<AdultGhostController>();
        if (!adult) continue;
        if (adult->GetState() != AdultGhostState::Patrol) continue;  // Patrol 상태인 유령만

        auto adultPos = adult->GetOwner()->GetTransform()->GetWorldPosition();
        float dist = (cryPos - adultPos).Length();

        if (dist < minDist)
        {
            minDist = dist;
            closestPatrolGhost = adult;
        }
    }

    if (closestPatrolGhost)
    {
        cout << "[BabyGhost_Cry] Sending cry signal to nearest Patrol AdultGhost" << endl;

        // 아기 유령 위치를 타겟으로 설정
        closestPatrolGhost->SetAITarget(babyGhost->GetOwner());
        closestPatrolGhost->chaseReason = ChaseReason::FromBabyCry;
        closestPatrolGhost->ChangeStateTo(AdultGhostState::Chase);
    }
    else
    {
        cout << "[BabyGhost_Cry] No Patrol AdultGhost found, keep crying" << endl;
    }
}

void BabyGhost_Cry::ChangeStateLogic()
{
    // 어른 유령이 다가와서 달래기 시작하면 
    // careTimer 작동 시작 
    // careTimer > careDelay 가 되면 -> Patrol 상태로 전환 

    // 가까운 어른 유령이 다가오면 careTimer 증가
    auto adultGhosts = SceneSystem::Instance().GetCurrentScene()->GetGameObjectsByName("Ghost_Adult");

    bool beingCared = false;

    for (auto* go : adultGhosts)
    {
        auto* adult = go->GetComponent<AdultGhostController>();
        if (!adult) continue;

        if (adult->GetTarget() == babyGhost->GetOwner() && adult->GetState() == AdultGhostState::Chase)
        {
            beingCared = true;
            break;
        }
    }

    if (beingCared)
    {
        careTimer += 0.016f; // Update에서 deltaTime 대신 간단히 예시
        if (careTimer >= careDelay)
        {
            babyGhost->ChangeState(BabyGhostState::Patrol);
        }
    }
}

void BabyGhost_Cry::Update(float deltaTime)
{
   // careTimer += deltaTime;
}

void BabyGhost_Cry::FixedUpdate(float deltaTime)
{
}

void BabyGhost_Cry::Exit()
{
    careTimer = 0.0f;
    babyGhost->agent->externalControl = false;
}