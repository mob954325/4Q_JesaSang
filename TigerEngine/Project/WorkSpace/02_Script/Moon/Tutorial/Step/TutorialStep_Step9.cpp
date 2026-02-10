#include "TutorialStep_Step9.h"

#include "System/InputSystem.h"


void TutorialStep_Step9::Enter()
{
    isDone = false;
    stepTimer = 0.0f;
    phase = Step9Phase::Monologue;

    std::cout << "[Step9] Enter" << std::endl;
}

void TutorialStep_Step9::Update(float deltaTime)
{
    stepTimer += deltaTime;

    switch (phase)
    {
    case Step9Phase::Monologue:

        if (stepTimer >= delayStart)
        {
            phase = Step9Phase::WaitInput;
        }
        break;


    case Step9Phase::WaitInput:
        phase = Step9Phase::PlayerOut;
        break;


    case Step9Phase::PlayerOut:
        phase = Step9Phase::Done;
        break;


    case Step9Phase::Done:
        isDone = true;
        break;
    }
}

bool TutorialStep_Step9::IsComplete()
{
    return isDone;
}

void TutorialStep_Step9::Exit()
{
    std::cout << "[Step9] Exit " << std::endl;
}


// ---------------------------------------------------------------------


void TutorialStep_Step9::Monologue()
{

}

void TutorialStep_Step9::WaitInput()
{

}

void TutorialStep_Step9::PlayerOut()
{

}