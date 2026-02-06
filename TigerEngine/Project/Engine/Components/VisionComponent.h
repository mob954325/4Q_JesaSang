#pragma once
#include "../Object/Component.h"
#include "../Util/CollisionLayer.h"

struct VisionDebugParam
{
    float fov = 0.f;
    float dist = 0.f;
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
    float eyeHigh = 50.0f;

public:
    // register enable
    void Enable_Inner() override;
    void Disable_Inner() override;

    // 시야 확인 ( target이 있는지 확인) 
    bool CheckVision(GameObject* target, float fov, float maxDistance);

    // 시야 디버그 
    void DrawDebugVision();
};