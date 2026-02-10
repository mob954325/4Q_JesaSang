#include "TutorialStep_Step2.h"

#include "System/InputSystem.h"


void TutorialStep_Step2::Enter()
{
    isDone = false;
    step2Timer = 0.0f;
    phase = Step2Phase::Sound;

    std::cout << "[Step2] *BANG!* Sound from entrance!\n";
}

void TutorialStep_Step2::Update(float deltaTime)
{
    step2Timer += deltaTime;


    switch (phase)
    {
    case Step2Phase::Sound:

        if (step2Timer >= delayStep2)
        {
            phase = Step2Phase::Monologue;
        }
        break;



    case Step2Phase::Monologue:

        std::cout << "[Step2] \"..!! Mom's Comming?\"\n";
        phase = Step2Phase::WaitInput;
        break;



    case Step2Phase::WaitInput:

        if (
            Input::GetKeyDown(Keyboard::F) ||
            Input::GetKeyDown(Keyboard::Space) ||
            Input::GetMouseButtonDown(0) ||
            Input::GetKeyDown(Keyboard::Up) ||
            Input::GetKeyDown(Keyboard::Down) ||
            Input::GetKeyDown(Keyboard::Left) ||
            Input::GetKeyDown(Keyboard::Right)
            )
        {
            phase = Step2Phase::Done;
            // isDone = true;
        }
        break;



    case Step2Phase::Done:
        isDone = true;
        break;
    }
}

bool TutorialStep_Step2::IsComplete()
{
    return isDone;
}

void TutorialStep_Step2::Exit()
{
    std::cout << "[Step2] Exit " << std::endl;
}
