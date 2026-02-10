#include "TutorialStep_Step1.h"

#include "System/InputSystem.h"

void TutorialStep_Step1::Enter()
{
    isDone = false;
    step1Timer = 0.0f;
    phase = Step1Phase::Blink;

    std::cout << "[Step1] EYE - Blink Effect ing..." << std::endl;
}

void TutorialStep_Step1::Update(float deltaTime)
{
    step1Timer += deltaTime;

    switch (phase)
    {
    case Step1Phase::Blink:

        if (step1Timer >= delayStep1) // 아직 연출 시간
        {
            phase = Step1Phase::Monologue;
        }
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