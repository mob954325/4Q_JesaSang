#pragma once
#include "Components/ScriptComponent.h"
#include "Components/AgentComponent.h"
#include "Components/GridComponent.h"


class AdultGhostController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    AgentComponent* agent = nullptr;
    GridComponent* grid = nullptr;

public:
    void OnStart() override;
    void OnFixedUpdate(float dt) override;
};
