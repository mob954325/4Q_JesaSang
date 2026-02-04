#pragma once
#include "../Object/Component.h"
#include "../Util/CollisionLayer.h"

class VisionComponent : public Component
{
    RTTR_ENABLE(Component)

public:
    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

public:
    // register enable
    void Enable_Inner() override;
    void Disable_Inner() override;

    bool CheckVision(
        GameObject* target,
        float fovDeg,
        float viewDistance,
        CollisionMask targetMask,
        CollisionMask occlusionMask);
};