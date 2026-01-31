#pragma once
#include "Components/ScriptComponent.h"
#include "Components/FBXRenderer.h"
#include "../PlayerController.h"

/*
    [ InteractionZone Script Component ]

    플레이가 실제로 상호작용 가능한 범위
    - 실제 인터랙션 가능
*/

class InteractionZone : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // components
    PlayerController* player;

    // members..

public:
    // component process
    void OnStart() override;
    void OnUpdate(float delta) override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    // trigger event
    void OnTriggerEnter(PhysicsComponent* other) override;
    void OnTriggerStay(PhysicsComponent* other) override;
    void OnTriggerExit(PhysicsComponent* other) override;

private:
    // funcs..
};

