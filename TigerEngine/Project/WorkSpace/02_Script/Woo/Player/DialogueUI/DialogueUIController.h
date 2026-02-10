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
    Vector3 offset = { 50,0,0 };

    // 연출
    float dialogueDuration = 4.5f;
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

    // 텍스트 업데이트하면서 자동 연출. 이거만 쓰면 됨.
    void UpdateText(const wchar_t* s);      
};

