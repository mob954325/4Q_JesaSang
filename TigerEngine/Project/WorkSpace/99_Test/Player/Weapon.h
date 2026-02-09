#pragma once
#include "Object/Component.h"
#include "System/InputSystem.h"
#include "Components/ScriptComponent.h"
#include "../Engine/Components/AudioListenerComponent.h"

class Weapon : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    void OnInitialize() override;
    void OnEnable() override;
    void OnStart() override;
    void OnDisable() override;
    void OnDestory() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

private:
    AudioListenerComponent* listener{};

    Vector3 prevPos = {};
    bool hasPrev = false; // 최초 적용 확인용
};