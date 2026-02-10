#include "TutorialStep_Step5.h"

#include "System/InputSystem.h"


void TutorialStep_Step5::Enter()
{
    isDone = false;
    step5Timer = 0.0f;
    phase = Step5Phase::AdultInCome;

    std::cout << "[Step5] Enter" << std::endl;
}

void TutorialStep_Step5::Update(float deltaTime)
{
    step5Timer += deltaTime;

    switch (phase)
    {
    case Step5Phase::AdultInCome:

        if (step5Timer >= delayStep5)
        {
            phase = Step5Phase::LookAround;
        }
        break;


    case Step5Phase::LookAround:
        phase = Step5Phase::Monologue;
        break;

    case Step5Phase::Monologue:
        phase = Step5Phase::WaitInput;
        break;

    case Step5Phase::WaitInput:
        phase = Step5Phase::AdultOut;
        break;

    case Step5Phase::AdultOut:
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


// -------------------------------------------------------------

void TutorialStep_Step5::AdultInCome()
{

}

void TutorialStep_Step5::LookAround()
{

}

void TutorialStep_Step5::Monologue()
{

}

void TutorialStep_Step5::WaitInput()
{

}

void TutorialStep_Step5::AdultOut()
{

}