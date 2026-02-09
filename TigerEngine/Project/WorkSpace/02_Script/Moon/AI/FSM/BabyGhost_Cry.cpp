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

}

void BabyGhost_Cry::ChangeStateLogic()
{
    // 어른 유령이 다가와서 달래기 시작하면 
    // careTimer 작동 시작 
    // careTimer > careDelay 가 되면 -> Patrol 상태로 전환 


    babyGhost->ChangeState(BabyGhostState::Patrol);
}

void BabyGhost_Cry::Update(float deltaTime)
{
    careTimer += deltaTime;
}

void BabyGhost_Cry::FixedUpdate(float deltaTime)
{
}

void BabyGhost_Cry::Exit()
{
    careTimer = 0.0f;
    babyGhost->agent->externalControl = false;
}