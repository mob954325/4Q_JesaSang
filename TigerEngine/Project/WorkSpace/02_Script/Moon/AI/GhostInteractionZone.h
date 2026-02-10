#pragma once
#include "Components/ScriptComponent.h"
#include "Components/FBXRenderer.h"


// 플레이어가 고스트랑 충돌감지하는 영역 

class GhostInteractionZone : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

public:
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    void OnStart() override;
    void OnFixedUpdate(float delta) override;

    // trigger event
    void OnTriggerEnter(PhysicsComponent* other) override;

    void OnCCTTriggerEnter(CharacterControllerComponent* cct) override;
};