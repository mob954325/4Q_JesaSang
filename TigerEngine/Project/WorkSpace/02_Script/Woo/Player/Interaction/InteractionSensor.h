#pragma once
#include "Components/ScriptComponent.h"
#include "Components/FBXRenderer.h"

/*
    [ InteractionSensor Script Component ]

    플레이어와 상호작용 가능한 오브젝트가 주위에 있는지를 감지하는 범위
    - 실제 인터랙션 가능 x
    - UI 표시 등에 활용됨
*/

class InteractionSensor : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // components

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

