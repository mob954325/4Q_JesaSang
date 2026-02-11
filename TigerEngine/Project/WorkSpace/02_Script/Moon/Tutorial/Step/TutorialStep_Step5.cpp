#include "TutorialStep_Step5.h"

#include "System/InputSystem.h"
#include "EngineSystem/SceneSystem.h"
#include "System/TimeSystem.h"

#include "../../../Woo/Player/PlayerController.h"

// - 플레이어 렌더 끄고, hold상태에서 나오기 
// - 카메라와 다이얼로그 귀신에게 맞추기 
void TutorialStep_Step5::Enter()
{
    isDone = false;
    step5Timer = 0.0f;
    phase = Step5Phase::AdultInCome;
    phaseStarted = false;

    // 조작 불가 
    tutorialController->player_Obj->GetComponent<PlayerController>()->SetInputLock(true);
    
    // 플레이어 모습 감추기
    tutorialController->player_Obj->GetComponent<FBXRenderer>()->SetActive(false);

    targetA = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Adult_Target_A");
    targetB = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Adult_Target_B");

    // 어른 귀신의 위치를 targetA로 세팅 
    tutorialController->adultGhost_Obj->GetTransform()->SetPosition(targetA->GetComponent<Transform>()->GetWorldPosition());
    adultTranform = tutorialController->adultGhost_Obj->GetTransform();

    std::cout << "[Step5] Enter" << std::endl;
}

void TutorialStep_Step5::Update(float deltaTime)
{
    step5Timer += deltaTime;

    switch (phase)
    {
    case Step5Phase::AdultInCome:
        AdultInCome();
        break;

    case Step5Phase::LookAround:
        LookAround();
        break;

    case Step5Phase::Monologue1:
        Monologue1();
        break;

    case Step5Phase::WaitInput1:
        WaitInput1();
        break;

    case Step5Phase::Monologue2:
        Monologue2();
        break;

    case Step5Phase::WaitInput2:
        WaitInput2();
        break;

    case Step5Phase::AdultOut:
        AdultOut();
        break;

    case Step5Phase::Done:
        isDone = true;
        break;
    }
}

bool TutorialStep_Step5::IsComplete()
{
    return isDone;
}

void TutorialStep_Step5::Exit()
{
    std::cout << "[Step5] Exit " << std::endl;
}


// -------------------------------------------------------------

// 1. 어른 유령이 방으로 들어옴 & 화면 외각 프로즌 점점 심해짐 
void TutorialStep_Step5::AdultInCome()
{
    // TargetB로 이동 
    Vector3 target_b = targetB->GetTransform()->GetWorldPosition();
    Vector3 pos = tutorialController->adultGhost_Obj->GetTransform()->GetWorldPosition();
    Vector3 dir = target_b - pos;

    if (dir.Length() < 5.0f)
    {
        phase = Step5Phase::LookAround;
        return;
    }

    dir.Normalize();

    float dt = GameTimer::Instance().UnscaledDeltaTime();

    adultTranform->Translate(dir * speed * dt);

    // 이동 방향 바라보게 회전
    float yaw = atan2f(-dir.x, -dir.z);
    adultTranform->SetRotationY(yaw);
}


// 2. adultGhost_Obj 좌우로 두리번 두리번 거리기 
void TutorialStep_Step5::LookAround()
{
    if (!phaseStarted)
    {
        phaseStarted = true;
        step5Timer = 0.0f;
    }

    float dt = GameTimer::Instance().UnscaledDeltaTime();

    // 좌우로 살짝 회전
    float duration = 7.0f;

    // 회전 범위 (라디안)
    float angleRange = XMConvertToRadians(50.0f); 

    // 현재 시간 비율
    float t = step5Timer / duration;

    // sin으로 자연스럽게 왔다갔다
    float yaw = sinf(t * XM_2PI) * angleRange;

    adultTranform->SetRotationY(yaw);

    step5Timer += dt;

    if (step5Timer >= duration)
    {
        phaseStarted = false;
        step5Timer = 0.0f;
        phase = Step5Phase::Monologue1;
    }
}

void TutorialStep_Step5::Monologue1()
{
    if (!monologuePlayed)
    {
        // 독백 1
        std::cout << "[Step5] " << "어 여기 맛있는 냄새가 나는데?" << std::endl;
        monologuePlayed = true;
    }

    // 대화창은 tutorialController->dialogue 사용
    if (tutorialController->dialogue)
        tutorialController->dialogue->ShowDialogueHold(L"어 여기 맛있는 냄새가 나는데?");

    phase = Step5Phase::WaitInput1;
}

void TutorialStep_Step5::WaitInput1()
{
    if (
        Input::GetKeyDown(Keyboard::F) ||
        Input::GetKeyDown(Keyboard::Space) ||
        Input::GetMouseButtonDown(0)
        )
    {
        if (tutorialController->dialogue)
            tutorialController->dialogue->DialogueOnOff(false);

        phase = Step5Phase::Monologue2;
        monologuePlayed = false;
    }
}

void TutorialStep_Step5::Monologue2()
{
    if (!monologuePlayed)
    {
        std::cout << "[Step5] " << "아무것도 없네, 배고파서 헛 냄새를 맡았나.." << std::endl;
        monologuePlayed = true;
    }

    if (tutorialController->dialogue)
        tutorialController->dialogue->ShowDialogueHold(L"아무것도 없네, 배고파서 헛 냄새를 맡았나..");

    phase = Step5Phase::WaitInput2;
}

void TutorialStep_Step5::WaitInput2()
{
    if (
        Input::GetKeyDown(Keyboard::F) ||
        Input::GetKeyDown(Keyboard::Space) ||
        Input::GetMouseButtonDown(0)
        )
    {
        if (tutorialController->dialogue)
            tutorialController->dialogue->DialogueOnOff(false);

        phase = Step5Phase::AdultOut;
    }
}

// Target A의 위치로 이동 
void TutorialStep_Step5::AdultOut()
{
    // Target A로 이동 
    Vector3 target_a = targetA->GetTransform()->GetWorldPosition();
    Vector3 pos = tutorialController->adultGhost_Obj->GetTransform()->GetWorldPosition();
    Vector3 dir = target_a - pos;

    if (dir.Length() < 5.0f)
    {
        phase = Step5Phase::Done;
        return;
    }

    dir.Normalize();

    float dt = GameTimer::Instance().UnscaledDeltaTime();

    adultTranform->Translate(dir * speed * dt);

    // 이동 방향 바라보게 회전
    float yaw = atan2f(-dir.x, -dir.z);
    adultTranform->SetRotationY(yaw);
}