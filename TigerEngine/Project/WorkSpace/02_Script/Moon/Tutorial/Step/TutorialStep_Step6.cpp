#include "TutorialStep_Step6.h"

#include "System/InputSystem.h"


void TutorialStep_Step6::Enter()
{
    isDone = false;
    stepTimer = 0.0f;
    phase = Step6Phase::HideOut;

    std::cout << "[Step6] Enter" << std::endl;
}

void TutorialStep_Step6::Update(float deltaTime)
{
    stepTimer += deltaTime;

    switch (phase)
    {
    case Step6Phase::HideOut:

        if (stepTimer >= delayStart)
        {
            phase = Step6Phase::Monologue;
        }
        break;


    case Step6Phase::Monologue:
        phase = Step6Phase::WaitInput;
        break;


    case Step6Phase::WaitInput:
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


// -----------------------------------------------------------

void TutorialStep_Step6::HideOut()
{

}

void TutorialStep_Step6::Monologue()
{

}

void TutorialStep_Step6::WaitInput()
{

}