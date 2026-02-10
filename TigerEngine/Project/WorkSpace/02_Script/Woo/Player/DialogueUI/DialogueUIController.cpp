#include "DialogueUIController.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"

#include "EngineSystem/SceneSystem.h"
#include "System/InputSystem.h"
#include "System/TimeSystem.h"
#include "Object/GameObject.h"
#include "Components/RectTransform.h"
#include "Components/UI/Image.h"
#include "Components/UI/TextUI.h"

#include <directxtk/Keyboard.h>
#include <cwctype>
#include <string>


REGISTER_COMPONENT(DialogueUIController)

RTTR_REGISTRATION
{
    rttr::registration::class_<DialogueUIController>("DialogueUIController")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

namespace
{
    // 공백 제외 글자 수 세기
    int CountNonSpaceChars(const wchar_t* s)
    {
        if (!s) return 0;

        int count = 0;
        for (const wchar_t* p = s; *p; ++p)
        {
            if (!iswspace(*p))
                ++count;
        }
        return count;
    }

    // 글자 수에 따른 말풍선 크기 단계 선택
    // - 50  : 5글자 이하
    // - 100 : 10글자 이하
    // - 150 : 15글자 이하
    // - 200 : 20글자 이하
    // - 250 : 25글자 이하
    float SelectBubbleWidthByLen(int nonSpaceLen)
    {
        if (nonSpaceLen <= 5)  return 50.0f;
        if (nonSpaceLen <= 10) return 100.0f;
        if (nonSpaceLen <= 15) return 150.0f;
        if (nonSpaceLen <= 20) return 200.0f;
        return 250.0f;
    }
}


void DialogueUIController::OnStart()
{
    targetTr = GetOwner()->GetChildByName("DialoguePoint");

    auto curScene = SceneSystem::Instance().GetCurrentScene();

    auto parentGO = curScene->GetGameObjectByName("UI_PlayerDialogue");
    auto imgGO = curScene->GetGameObjectByName("Image_Dialogue");
    auto textGO = curScene->GetGameObjectByName("Text_Dialoaue"); // 오브젝트 이름 실제 값 확인 필요

    dialogueParent = parentGO ? parentGO->GetTransform() : nullptr;
    dialogueRect = imgGO ? imgGO->GetComponent<RectTransform>() : nullptr;
    text_dialogue = textGO ? textGO->GetComponent<TextUI>() : nullptr;
    
    if (!targetTr || !dialogueParent || !text_dialogue || !dialogueRect)
    {
        std::cout << "[DialogueUIController] Missing Components!" << std::endl;
    }

    // init
    DialogueOnOff(false);
}

void DialogueUIController::OnUpdate(float delta)
{
    if (!targetTr || !dialogueParent || !text_dialogue)
        return;

    // position trace
    dialogueParent->SetPosition(targetTr->GetWorldPosition() + offset);

    // 연출
    if (isDialogueOn)
    {
        float unscaledDelta = GameTimer::Instance().UnscaledDeltaTime();
        dialogueTimer += unscaledDelta;
        if (dialogueTimer >= dialogueDuration)
        {
            dialogueTimer = 0.0f;
            isDialogueOn = false;
            DialogueOnOff(false);

            // 게임 재개
            GameTimer::Instance().SetTimeScale(1.0f);
        }
    }

    // --- test --- 
    // TODO :: Delete
    //if (Input::GetKeyDown(Keyboard::P))
    //{
    //    DialogueToggle();
    //}
    //
    //if (Input::GetKeyDown(Keyboard::Q))
    //    UpdateText(L"123 45");
    //if (Input::GetKeyDown(Keyboard::W))
    //    UpdateText(L"123 456 789");
    //if (Input::GetKeyDown(Keyboard::E))
    //    UpdateText(L"12312 45612 78912");
    //if (Input::GetKeyDown(Keyboard::R))
    //    UpdateText(L"12312 45612 78912 12345");
    //if (Input::GetKeyDown(Keyboard::T))
    //    UpdateText(L"12312 45612 78912 12345 12345");
}

void DialogueUIController::OnDestory()
{

}

nlohmann::json DialogueUIController::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void DialogueUIController::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void DialogueUIController::DialogueOnOff(bool flag)
{
    if (!dialogueParent) return;

    // On -> 자동종료 트리거
    isDialogueOn = flag;
    if (flag) dialogueTimer = 0.0f;

    dialogueParent->GetOwner()->SetActive(flag);
}

void DialogueUIController::DialogueToggle()
{
    if (!dialogueParent) return;
    DialogueOnOff(!dialogueParent->GetOwner()->GetActiveSelf());
}

void DialogueUIController::ShowDialogueText(const wchar_t* s)
{
    if (!text_dialogue || !s) return;

    // text길이에 따른 image size 조절
    const int len = CountNonSpaceChars(s);
    const float w = SelectBubbleWidthByLen(len);

    if (dialogueRect)
    {
        Vector2 size = dialogueRect->GetSize();
        size.x = w;
        dialogueRect->SetSize(size);
    }

    // uapte text
    text_dialogue->SetText(std::wstring(s));

    // 자동 연출 시작
    DialogueOnOff(true);
}

void DialogueUIController::ShowInteractionHintAndPause(const wchar_t* s)
{
    if (!text_dialogue || !s) return;

    // text길이에 따른 image size 조절
    const int len = CountNonSpaceChars(s);
    const float w = SelectBubbleWidthByLen(len);

    if (dialogueRect)
    {
        Vector2 size = dialogueRect->GetSize();
        size.x = w;
        dialogueRect->SetSize(size);
    }

    // uapte text
    text_dialogue->SetText(std::wstring(s));

    // 게임 정지
    GameTimer::Instance().SetTimeScale(0.0);

    // 자동 연출 시작
    DialogueOnOff(true);
}
