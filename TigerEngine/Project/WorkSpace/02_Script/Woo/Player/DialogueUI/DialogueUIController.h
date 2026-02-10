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
    Image* image_dialogue = nullptr;
    TextUI* text_dialogue = nullptr;

    // offset
    Vector3 offset = { 50,0,0 };

    
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
};

