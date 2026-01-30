#pragma once
#include "Components/ScriptComponent.h"

/*
    [ OcclusionFader Script Component ]

*/

class OcclusionFader : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // TODO :: members


public:
    // component process
    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;
    void OnDestory() override;

    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

public:
    // funcs..
};

