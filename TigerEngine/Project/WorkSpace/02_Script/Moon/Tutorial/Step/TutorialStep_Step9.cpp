#include "TutorialStep_Step9.h"

#include "System/InputSystem.h"

#include "../../../Woo/Player/PlayerController.h"

void TutorialStep_Step9::Enter()
{
    isDone = false;
    stepTimer = 0.0f;
    phase = Step9Phase::Monologue;

    // 조작 불가 
    tutorialController->player_Obj->GetComponent<PlayerController>()->SetInputLock(true);

    // 아니면 연출 캐릭터 등장 

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
            Monologue();
            phase = Step9Phase::PlayerOut;
        }
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
    static const wchar_t* lines[] =
    {
        L"큰일이네, 제사상에 과일은 꼭 올라가야 되는데",
        L"분명 어딘가 제사음식이 남아 있을거야!"
    };

    if (tutorialController->dialogue)
        tutorialController->dialogue->ShowDialogueHold(lines[monoIndex]);

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
        monoIndex++;

        if (tutorialController->dialogue)
            tutorialController->dialogue->DialogueOnOff(false);

        if (monoIndex >= 2)
        {
            phase = Step9Phase::PlayerOut;
            phaseStarted = false;
        }
    }
}


void TutorialStep_Step9::PlayerOut()
{
    if (!phaseStarted)
    {
        // 진짜 플레이어 Off
        // 가짜 플레이어 On 

        // 가짜 플레이어가 Move하면서 이동하는 연출 

        phaseStarted = true;
    }
}