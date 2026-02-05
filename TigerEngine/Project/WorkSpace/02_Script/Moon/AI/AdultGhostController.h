#pragma once
#include "Components/ScriptComponent.h"
#include "Components/AgentComponent.h"
#include "Components/GridComponent.h"
#include "Components/VisionComponent.h"
#include "Util/CollisionLayer.h"

class AdultGhostController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    AgentComponent* agent = nullptr;
    GridComponent* grid = nullptr;
    VisionComponent* vision = nullptr;

public:
    void OnStart() override;
    void OnFixedUpdate(float dt) override;

    // 감지 대상 물체의 레이어 
    CollisionMask targetMask = (CollisionMask) CollisionLayer::Player;
    
    // 시야를 가릴 수 있는 물체의 레이어  
    CollisionMask occlusionMask =
        CollisionLayer::World |
        CollisionLayer::Default |
        CollisionLayer::Ball;
};
