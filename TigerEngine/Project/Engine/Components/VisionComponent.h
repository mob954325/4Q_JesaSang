#pragma once
#include "../Object/Component.h"
#include "../Util/CollisionLayer.h"

struct VisionDebugParam
{
    float fov = 0.f;
    float dist = 0.f;
    CollisionMask occlusionMask = 0;
    bool valid = false;
};


class VisionComponent : public Component
{
    RTTR_ENABLE(Component)

public:
    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

private:
    VisionDebugParam m_LastDebug;

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

    // void DrawDebugVision(float fovDeg, float viewDistance, CollisionMask occlusionMask);
    void DrawDebugVision();
};