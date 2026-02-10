#pragma once
#include "Components/ScriptComponent.h"

class GameObject;
class RectTransform;
class Image;
class TextUI;

/*
     [ DialogueUIController Script Component ]

     DialoguePoint 추적하여 위치를 업데이트하고,
     UI 관련 유틸 함수를 제공하는 컴포넌트입니다.

     여기서 해도 되나 싶지만 기믹설명 다이얼로그 연출시 게임시간까지 잠깐 멈춥니다 ^0^
*/

class DialogueUIController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // Target
    Transform* targetTr = nullptr;      // DialoguePoint

    // UI
    Transform* dialogueParent = nullptr;
    RectTransform* dialogueRect = nullptr;
    TextUI* text_dialogue = nullptr;

    // offset
    Vector3 offset = { 0, 0, 0 };

    // 연출
    float dialogueDuration = 1.8f;
    float dialogueTimer = 0.0f;
    bool isDialogueOn = false;
    
public:
    // Component process
    void OnStart() override;
    void OnUpdate(float delta) override;
    void OnDestory() override;

    // Json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

public:
    // UI Util Funcs
    void DialogueOnOff(bool flag);
    void DialogueToggle();

    // 텍스트 Upate 후 dialogueDuration뒤 자동 off
    void ShowDialogueText(const wchar_t* s);      

    // 텍스트 Upate와 동시에 인게임 정지후 dialogueDuration뒤 자동 off
    void ShowInteractionHintAndPause(const wchar_t* s);
};

