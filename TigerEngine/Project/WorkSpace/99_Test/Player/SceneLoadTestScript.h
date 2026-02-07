#pragma once
#include "System/InputSystem.h"
#include "Components/ScriptComponent.h"

/// <summary>
/// 클라이언트 컴포넌트 테스트용 클래스.
/// </summary>
class SceneLoadTestScript : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:

    void OnInitialize() override;
    void OnEnable() override;
    void OnStart() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

private:
    int index = 0;
};

