#include "TutorialStep_Step8.h"

#include "System/InputSystem.h"


void TutorialStep_Step8::Enter()
{
    isDone = false;
    stepTimer = 0.0f;
    phase = Step8Phase::FreePlay;

    std::cout << "[Step8] Enter" << std::endl;
}

void TutorialStep_Step8::Update(float deltaTime)
{
    stepTimer += deltaTime;

    switch (phase)
    {
    case Step8Phase::FreePlay:

        if (stepTimer >= delayStart)
        {
            phase = Step8Phase::Monologue;
        }
        break;


    case Step8Phase::Monologue:
        phase = Step8Phase::Done;
        break;


    case Step8Phase::Done:
        isDone = true;
        break;
    }
}

bool TutorialStep_Step8::IsComplete()
{
    return isDone;
}

void TutorialStep_Step8::Exit()
{
    std::cout << "[Step8] Exit " << std::endl;
}
