#include "TutorialStep_Step4.h"

#include "System/InputSystem.h"
#include "System/TimeSystem.h"

#include "EngineSystem/SceneSystem.h"
#include "../../../Woo/Player/PlayerController.h"

void TutorialStep_Step4::Enter()
{
    isDone = false;
    step4Timer = 0.0f;
    phase = Step4Phase::AdultInComeA;
    phaseStarted = false;

    hideObject = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("HideObject");
    targetA = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Adult_Target_A");
    targetB = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Adult_Target_B");

    // 귀신 위치 초기화 
    tutorialController->player_Obj->GetTransform()->SetPosition(tutorialController->InitPlayerPosition);
    adultTranform = tutorialController->player_Obj->GetTransform();

    // 플레이어 위치 초기화
    tutorialController->adultGhost_Obj->GetTransform()->SetPosition(tutorialController->InitAdultPosition);
    adultTranform = tutorialController->adultGhost_Obj->GetTransform();

    // 조작 불가 
    tutorialController->player_Obj->GetComponent<PlayerController>()->SetInputLock(true);

    std::cout << "[Step4] Enter" << std::endl;
}

void TutorialStep_Step4::Update(float deltaTime)
{
    step4Timer += deltaTime;

    switch (phase)
    {
    case Step4Phase::AdultInComeA: AdultInComeA(); break;
    case Step4Phase::AdultInComeB: AdultInComeB(); break;
    case Step4Phase::AdultArrive:  AdultArrive();  break;
    case Step4Phase::Fail:
        std::cout << "[Step4] Fail → Restart\n";
        // TODO : 3초간 실패 효과 
        Enter();  // 리셋
        break;
    case Step4Phase::Clear:
        isDone = true;
        break;
    }
}

bool TutorialStep_Step4::IsComplete()
{
    return isDone;
}

void TutorialStep_Step4::Exit()
{
    std::cout << "[Step4] Exit " << std::endl;
}



// ------------------------------------------------------------------



// 1. 조상 유령 접근 + 거리에 따라서 화면 Frozen 
void TutorialStep_Step4::AdultInComeA()
{
    // 조작 가능 
    if (!phaseStarted)
    {
        phaseStarted = true;
        tutorialController->player_Obj->GetComponent<PlayerController>()->SetInputLock(false);
    }

    // 만약 플레이어가 Hide 상태면 바로 Clear 
    if (tutorialController->player_Obj->GetComponent<PlayerController>()->GetPlayerState() == PlayerState::Hide)
    {
        phase = Step4Phase::Clear;
        return;
    }

    // TargetA 와 어른 유령의 거리가 일정이상 가까워지면 TargetB로 이동 
    Vector3 target_a = targetA->GetTransform()->GetWorldPosition();
    Vector3 pos = tutorialController->adultGhost_Obj->GetTransform()->GetWorldPosition();
    Vector3 dir = target_a - pos; 

    if (dir.Length() < 5.0f)
    {
        phase = Step4Phase::AdultInComeB;
        return;
    }

    dir.Normalize();

    float dt = GameTimer::Instance().UnscaledDeltaTime();

    adultTranform->Translate(dir * speed * dt);

    // 이동 방향 바라보게 회전
    float yaw = atan2f(-dir.x, -dir.z);
    adultTranform->SetRotationY(yaw);
}


void TutorialStep_Step4::AdultInComeB()
{
    // 만약 플레이어가 Hide 상태면 바로 Clear 
    if (tutorialController->player_Obj->GetComponent<PlayerController>()->GetPlayerState() == PlayerState::Hide)
    {
        phase = Step4Phase::Clear;
        return;
    }
    
    // TargetB로 이동 
    Vector3 target_b = targetB->GetTransform()->GetWorldPosition();
    Vector3 pos = tutorialController->adultGhost_Obj->GetTransform()->GetWorldPosition();
    Vector3 dir = target_b - pos;

    if (dir.Length() < 5.0f)
    {
        phase = Step4Phase::AdultArrive;
        return;
    }

    dir.Normalize();

    float dt = GameTimer::Instance().UnscaledDeltaTime();

    adultTranform->Translate(dir * speed * dt);

    // 이동 방향 바라보게 회전
    float yaw = atan2f(-dir.x, -dir.z);
    adultTranform->SetRotationY(yaw);
}


void TutorialStep_Step4::AdultArrive()
{
    if (tutorialController->player_Obj->GetComponent<PlayerController>()->GetPlayerState() == PlayerState::Hide)
    {
        phase = Step4Phase::Clear;
        return;
    }
    else
    {
        Vector3 target = tutorialController->player_Obj->GetTransform()->GetWorldPosition();  // 플레이어 위치 
        Vector3 pos = tutorialController->adultGhost_Obj->GetTransform()->GetWorldPosition(); // 어른 유령 
        Vector3 dir = target - pos;

        if (dir.Length() < 5.0f)
        {
            phase = Step4Phase::Fail;
            return;
        }

        dir.Normalize();

        float dt = GameTimer::Instance().UnscaledDeltaTime();

        adultTranform->Translate(dir * fast_speed * dt);

        // 이동 방향 바라보게 회전
        float yaw = atan2f(-dir.x, -dir.z);
        adultTranform->SetRotationY(yaw);
    }
}