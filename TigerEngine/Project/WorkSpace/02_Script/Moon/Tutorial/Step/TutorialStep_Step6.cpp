#include "TutorialStep_Step6.h"

#include "System/InputSystem.h"


void TutorialStep_Step6::Enter()
{
    isDone = false;
    stepTimer = 0.0f;
    phase = Step6Phase::First;

    std::cout << "[Step6] Enter" << std::endl;
}

void TutorialStep_Step6::Update(float deltaTime)
{
    stepTimer += deltaTime;

    switch (phase)
    {
    case Step6Phase::First:

        if (stepTimer >= delayStart)
        {
            phase = Step6Phase::Second;
        }
        break;


    case Step6Phase::Second:
        phase = Step6Phase::Done;
        break;


    case Step6Phase::Done:
        isDone = true;
        break;
    }
}

bool TutorialStep_Step6::IsComplete()
{
    return isDone;
}

void TutorialStep_Step6::Exit()
{
    std::cout << "[Step6] Exit " << std::endl;
}
