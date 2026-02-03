#pragma once
#include "../Object/Component.h"
#include "../EngineSystem/GridSystem.h"

class CharacterControllerComponent;

class AgentComponent : public Component
{
    RTTR_ENABLE(Component)

public:
    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    AgentComponent() = default;
    ~AgentComponent();

    CharacterControllerComponent* cct = nullptr;

    // [ 런타임 전용 ]
    int cx = 0;
    int cy = 0;
    int targetCX = 0;
    int targetCY = 0;
    bool hasTarget = false;

    // [ 직렬화 대상 ]
    float reachDist = 20.0f;
    float patrolSpeed = 1.5f; // 상태마다 변경될 예정 

public:
    void OnInitialize() override;
    void OnStart() override;
    void OnFixedUpdate(float dt) override;

    // register enable
    void Enable_Inner() override;
    void Disable_Inner() override;

    void PickRandomTarget();
    void MoveAgent(const Vector3& dir, float speed, float dt);
};
