#pragma once
#include "Components/ScriptComponent.h"
#include "Components/AnimationController.h"
#include "Components/FBXData.h"

class PlayerAnimationTest : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    AnimationController* animController = nullptr;
    FBXData* fbxData = nullptr;

    float elapsedTime = 0.0f;  // 시간 측정용

public:
    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;
    void OnFixedUpdate(float dt) override;
};
