#include "TutorialStep_Step4.h"

#include "System/InputSystem.h"


void TutorialStep_Step4::Enter()
{
    isDone = false;
    step4Timer = 0.0f;
    phase = Step4Phase::First;

    std::cout << "[Step4] Enter" << std::endl;
}

void TutorialStep_Step4::Update(float deltaTime)
{
    step4Timer += deltaTime;

    switch (phase)
    {
    case Step4Phase::First:

        if (step4Timer >= delayStep4)
        {
            phase = Step4Phase::Second;
        }
        break;


    case Step4Phase::Second:
        phase = Step4Phase::Done;
        break;


    case Step4Phase::Done:
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
