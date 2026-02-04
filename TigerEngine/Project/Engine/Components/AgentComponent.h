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

    int cx = 0, cy = 0;             // 현재 위치 (중앙 기준)
    int targetCX = 0, targetCY = 0; // 목표 위치 (중앙 기준)
    bool hasTarget = false;

    float reachDist = 20.0f;  // 목표와의 거리 
    float patrolSpeed = 1.0f; // 상태마다 변경될 예정 

    std::vector<std::pair<int, int>> path; // A* 경로 저장 (그리드 좌표)

public:
    void OnInitialize() override;
    void OnStart() override;
    void OnFixedUpdate(float dt) override;

    void PickRandomTarget();
    void MoveAgent(const Vector3& dir, float speed, float dt);
};
