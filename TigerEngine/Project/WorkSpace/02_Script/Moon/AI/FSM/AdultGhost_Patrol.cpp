#include "AdultGhost_Patrol.h"

#include "Components/VisionComponent.h"
#include "EngineSystem/SceneSystem.h"

#include "../../../Woo/Object/HideObject.h"


void AdultGhost_Patrol::Enter()
{
    cout << "[AdultGhost_Patrol] Enter Patrol State" << endl;

    agent = adultGhost->agent;
    patrolTimer = 0.0f;

    agent->patrolSpeed = 0.8f;  // Patrol 속도 
    agent->SetWaitTime(3.0f);   // 목표 지점에서 대기 시간 

    // 아직 이동 중이 아니라면 랜덤 목표 설정
    if (!agent->hasTarget && !agent->isWaiting)
        agent->PickRandomTarget();
}

void AdultGhost_Patrol::ChangeStateLogic()
{
    // 상태 진입 직후 바로 바뀌는 현상 방지 (임시)
    if (patrolTimer < forcePatrolTime)
        return;

    // 1. 시야 감지 : 플레이어 감지 
    if (adultGhost->IsSeeing(adultGhost->GetAITarget()))
    {
        std::cout << "[AdultGhost_Patrol] Ghost is Seeing PLAYER !" << std::endl;
        adultGhost->ChangeState(AdultGhostState::Chase);
        return;
    }

    // 2. 기척 감지 
    if (adultGhost->IsPlayerInSenseRange())
    {
        std::cout << "[AdultGhost_Patrol] PLAYER FOUND (Sense)!" << std::endl;
        adultGhost->ChangeState(AdultGhostState::Search);
        return;
    }

    // 3. HideObject 시야 체크
    UpdateHideObjectVision();
}

void AdultGhost_Patrol::Update(float deltaTime)
{
    patrolTimer += deltaTime;
}

void AdultGhost_Patrol::FixedUpdate(float deltaTime)
{
}

void AdultGhost_Patrol::Exit()
{
    auto* hideObj = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("HideObject");
    if (hideObj)
    {
        auto* hideComp = hideObj->GetComponent<HideObject>();
        if (hideComp && adultGhost->hideLookRegistered)
        {
            hideComp->UnregisterAILook(adultGhost);
            adultGhost->hideLookRegistered = false;
            adultGhost->curSeeingHideObject = nullptr;
        }
    }

    agent->hasTarget = false;
    agent->path.clear();
    agent->isWaiting = false;

    cout << "[AdultGhost_Patrol] Exit" << endl;
}


// --------------------------------------------------------

void AdultGhost_Patrol::UpdateHideObjectVision()
{
    auto* hideObj = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("HideObject");

    if (!hideObj) return;

    auto* hideComp = hideObj->GetComponent<HideObject>();
    if (!hideComp) return;

    bool nowSeeing = adultGhost->IsSeeing(hideObj);

    if (nowSeeing && !adultGhost->hideLookRegistered)
    {
        hideComp->RegisterAILook(adultGhost);
        adultGhost->hideLookRegistered = true;
        adultGhost->curSeeingHideObject = hideObj;
    }
    else if (!nowSeeing && adultGhost->hideLookRegistered)
    {
        hideComp->UnregisterAILook(adultGhost);
        adultGhost->hideLookRegistered = false;
        adultGhost->curSeeingHideObject = nullptr;
    }
}