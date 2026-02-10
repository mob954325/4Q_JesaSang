#include "TutorialStep_Step1.h"

#include "System/InputSystem.h"
#include "EngineSystem/SceneSystem.h"
#include <Components/RectTransform.h>

float Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}


void TutorialStep_Step1::Enter()
{
    isDone = false;
    step1Timer = 0.0f;
    phase = Step1Phase::Blink;


    Blink_Top = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Blink_Top"); // rectTransform 축 -800 
    Blink_Bottom = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Blink_Bottom"); // rectTransform 축 800 
}

void TutorialStep_Step1::Update(float deltaTime)
{
    step1Timer += deltaTime;

    switch (phase)
    {
    case Step1Phase::Blink:

        // 5초 동안 눈깜빡임 
        Blink();

        // 연출 끝나면 전환 
        if (step1Timer >= BlinkTime)  phase = Step1Phase::Monologue;

        break;


    case Step1Phase::Monologue:

        std::cout << "[Step1] \" Umm... Did I just fall asleep.. \" " << std::endl;
        phase = Step1Phase::WaitInput; // 바로 입력 대기 상태로 전환
        break;


    case Step1Phase::WaitInput:
        // 아무 입력 받으면 넘어가기 
        if (
            Input::GetKeyDown(Keyboard::F) ||
            Input::GetKeyDown(Keyboard::Space) ||
            Input::GetMouseButtonDown(0) ||
            Input::GetKeyDown(Keyboard::Up) ||
            Input::GetKeyDown(Keyboard::Down) ||
            Input::GetKeyDown(Keyboard::Left) ||
            Input::GetKeyDown(Keyboard::Right)
            )
        {
            phase = Step1Phase::Done;
        }
        break;


    case Step1Phase::Done:
        isDone = true;
        break;
    }
}

bool TutorialStep_Step1::IsComplete()
{
    return isDone;
}

void TutorialStep_Step1::Exit()
{
    std::cout << "[Step1] Exit " << std::endl;
}


// ------------------

// Image 2개로 눈 깜빡이는 것 같은 연출 주기
// - Blink_Top    : (윗 눈꺼풀)   -800에서 -500 까지 이동 
// - Blink_Bottom : (아래 눈꺼풀)  800에서  500 까지 이동 
void TutorialStep_Step1::Blink()
{
    if (!Blink_Top || !Blink_Bottom) return;

    auto* topRect = Blink_Top->GetComponent<RectTransform>();
    auto* botRect = Blink_Bottom->GetComponent<RectTransform>();
    if (!topRect || !botRect) return;

    const float BlinkDelay = 2.0f;
    if (step1Timer < BlinkDelay) return;

    float t = step1Timer - BlinkDelay;

    Vector3 topPos = topRect->GetPos();
    Vector3 botPos = botRect->GetPos();

    auto Move = [&](float localT, float duration,
        float topFrom, float topTo,
        float botFrom, float botTo)
        {
            float p = std::clamp(localT / duration, 0.0f, 1.0f);
            topPos.y = Lerp(topFrom, topTo, p);
            botPos.y = Lerp(botFrom, botTo, p);
        };

    if (t < 1.0f)
        Move(t, 1.0f, -400, -700, 400, 700);
    else if (t < 2.0f)
        Move(t - 1, 1.0f, -700, -600, 700, 600);
    else if (t < 3.0f)
        Move(t - 2, 1.0f, -600, -800, 600, 800);
    else if (t < 4.0f)
        Move(t - 3, 1.0f, -800, -700, 800, 700);
    else if (t < 5.0f)
        Move(t - 4, 1.0f, -700, -1100, 700, 1100);

    topRect->SetPos(topPos);
    botRect->SetPos(botPos);
}


void TutorialStep_Step1::Monologue()
{

}

void TutorialStep_Step1::WaitInput()
{

}