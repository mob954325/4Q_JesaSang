#pragma once
#include "Components/ScriptComponent.h"
#include <directxtk/Keyboard.h>

class GameObject;
class RectTransform;
class Image;

/*
    [ Pause Manager Script Component ]

    인게임 일시정지 및 UI 컨트롤
*/

class PauseManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    Keyboard::Keys pause_key = Keyboard::Escape;
    bool isPauseOn = false;

    GameObject* pausePannel;

public:
    // component process
    void OnStart() override;
    void OnUpdate(float delta) override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
   void PauseOn();        // 인게임 일시정지
   void PauseOff();       // 인게임 이어하기
};

