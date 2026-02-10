#include "TutorialStep_Step3.h"

#include "System/InputSystem.h"


void TutorialStep_Step3::Enter()
{
    isDone = false;
    step3Timer = 0.0f;
    phase = Step3Phase::BabyInCome;

    std::cout << "[Step3] Enter" << std::endl;
}

void TutorialStep_Step3::Update(float deltaTime)
{
    step3Timer += deltaTime;

    switch (phase)
    {
    case Step3Phase::BabyInCome:

        if (step3Timer >= delayStep3)
        {
            phase = Step3Phase::TrunToPlayer;
        }
        break;


    case Step3Phase::TrunToPlayer:
        phase = Step3Phase::Second;
        break;

    case Step3Phase::Second:
        phase = Step3Phase::Done;
        break;


    case Step3Phase::Done:
        isDone = true;
        break;
    }
}

bool TutorialStep_Step3::IsComplete()
{
    return isDone;
}

void TutorialStep_Step3::Exit()
{
    std::cout << "[Step3] Exit " << std::endl;
}
