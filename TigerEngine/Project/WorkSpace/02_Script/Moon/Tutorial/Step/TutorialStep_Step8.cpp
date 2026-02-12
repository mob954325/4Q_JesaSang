#include "TutorialStep_Step8.h"

#include "System/InputSystem.h"
#include "System/TimeSystem.h"
#include "EngineSystem/SceneSystem.h"

#include "../SearchObject_Tutorial.h"
#include "../../../Woo/Player/PlayerController.h"

void TutorialStep_Step8::Enter()
{
    isDone = false;
    stepTimer = 0.0f;
    phase = Step8Phase::FreePlay;

    tutorialController->player_Obj->GetComponent<PlayerController>()->SetInputLock(false);

    auto obj = SceneSystem::Instance().GetCurrentScene() ->GetGameObjectByName("SearchObject_Tutorial");

    basket = obj->GetComponent<SearchObject_Tutorial>();

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
            FreePlay();
        }
        break;


    case Step8Phase::Monologue:
        Monologue();
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


// -----------------------------------------------------


void TutorialStep_Step8::FreePlay()
{
    // 만약 F키 상호작용을 해서 완료하면, Monologue로 넘어가기 
    if (!basket || basket->isCompleted)
        return;

    // 거리 체크
    Vector3 playerPos = tutorialController->player_Obj->GetTransform()->GetWorldPosition();
    Vector3 basketPos = basket->GetOwner()->GetTransform()->GetWorldPosition();

    float dist = (playerPos - basketPos).Length();

    if (dist < 120.0f)
    {
        basket->ShowUI(true);

        if (basket->UpdateInteraction(GameTimer::Instance().DeltaTime()))
        {
            phase = Step8Phase::Monologue;
        }
    }
    else
    {
        basket->ShowUI(false);
    }
}


void TutorialStep_Step8::Monologue()
{
    static const wchar_t* line = L"어라… 바구니에 과일이 없네?";

    tutorialController->dialogue->ShowDialogueHold(line);

    if (Input::GetKeyDown(Keyboard::F) || Input::GetMouseButtonDown(0))
    {
        tutorialController->dialogue->DialogueOnOff(false);
        phase = Step8Phase::Done;
    }
}