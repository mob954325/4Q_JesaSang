#pragma once
#include "System/InputSystem.h"
#include "Components/ScriptComponent.h"
#include "Weapon.h"
#include "Components/UI/Image.h"

/// <summary>
/// 클라이언트 컴포넌트 테스트용 클래스.
/// </summary>
class PlayerUI : public ScriptComponent
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
    Image* image{};
};

