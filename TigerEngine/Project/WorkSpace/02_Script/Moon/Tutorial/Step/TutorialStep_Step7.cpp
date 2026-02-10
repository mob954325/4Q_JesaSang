#include "TutorialStep_Step7.h"

#include "System/InputSystem.h"


void TutorialStep_Step7::Enter()
{
    isDone = false;
    stepTimer = 0.0f;
    phase = Step7Phase::First;

    std::cout << "[Step7] Enter" << std::endl;
}

void TutorialStep_Step7::Update(float deltaTime)
{
    stepTimer += deltaTime;

    switch (phase)
    {
    case Step7Phase::First:

        if (stepTimer >= delayStart)
        {
            phase = Step7Phase::Second;
        }
        break;


    case Step7Phase::Second:
        phase = Step7Phase::Done;
        break;


    case Step7Phase::Done:
        isDone = true;
        break;
    }
}

bool TutorialStep_Step7::IsComplete()
{
    return isDone;
}

void TutorialStep_Step7::Exit()
{
    std::cout << "[Step7] Exit " << std::endl;
}
