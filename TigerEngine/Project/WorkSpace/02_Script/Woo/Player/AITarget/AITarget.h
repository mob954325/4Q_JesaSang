#pragma once
#include "Components/ScriptComponent.h"
#include "Components/FBXRenderer.h"
#include "../PlayerController.h"

/*
    [ AITarget Script Component ]

    
*/

class AITarget : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // components
    Transform* tr;

    // members..

public:
    // component process
    void OnStart() override;
    void OnFixedUpdate(float delta) override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);
};

