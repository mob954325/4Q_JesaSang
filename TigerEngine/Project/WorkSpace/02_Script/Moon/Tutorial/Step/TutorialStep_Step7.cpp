#include "TutorialStep_Step7.h"

#include "System/InputSystem.h"


void TutorialStep_Step7::Enter()
{
    isDone = false;
    stepTimer = 0.0f;
    phase = Step7Phase::PlayerToJesasang;

    std::cout << "[Step7] Enter" << std::endl;
}

void TutorialStep_Step7::Update(float deltaTime)
{
    stepTimer += deltaTime;

    switch (phase)
    {
    case Step7Phase::PlayerToJesasang:

        if (stepTimer >= delayStart)
        {
            phase = Step7Phase::CameraToJesasang;
        }
        break;


    case Step7Phase::CameraToJesasang:
        phase = Step7Phase::Monologue;
        break;


    case Step7Phase::Monologue:
        phase = Step7Phase::CameraToFruit;
        break;


    case Step7Phase::CameraToFruit:
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


// --------------------------------------------------------------------


void TutorialStep_Step7::PlayerToJesasang()
{

}

void TutorialStep_Step7::CameraToJesasang()
{

}

void TutorialStep_Step7::Monologue()
{

}

void TutorialStep_Step7::CameraToFruit()
{

}