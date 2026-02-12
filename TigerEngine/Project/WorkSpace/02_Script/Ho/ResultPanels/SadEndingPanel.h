#pragma once
#include "../../../Engine/Components/ScriptComponent.h"
#include "../../../Engine/Components/UI/Image.h"
#include "../../../Engine/Components/UI/TextUI.h"

class SadEndingPanel : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    void OnStart() override;
    void OnUpdate(float dt) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

private:
    Image* image{};
    TextUI* text{};

    float maxTime = 0.5f;
    float timer = 0.0f;
    bool waitingAnyKey = false;    // maxTime 이후 입력 대기 상태
    bool handled = false;          // 한 번만 처리
};
