#include "TutorialStep_Step5.h"

#include "System/InputSystem.h"


void TutorialStep_Step5::Enter()
{
    isDone = false;
    step5Timer = 0.0f;
    phase = Step5Phase::First;

    std::cout << "[Step5] Enter" << std::endl;
}

void TutorialStep_Step5::Update(float deltaTime)
{
    step5Timer += deltaTime;

    switch (phase)
    {
    case Step5Phase::First:

        if (step5Timer >= delayStep5)
        {
            phase = Step5Phase::Second;
        }
        break;


    case Step5Phase::Second:
        phase = Step5Phase::Done;
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
