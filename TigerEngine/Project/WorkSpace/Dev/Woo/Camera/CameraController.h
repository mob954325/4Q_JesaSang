#pragma once
#include "Components/ScriptComponent.h"
#include <directxtk/SimpleMath.h>

class Transform;
class Camera;

/*
     [ Camera Controller Script Component ]
     
     dead zone 안에 있을때는 look focusing만, 
     dead zone 밖으로 나가면 position tracking (dead zone까지만)

     1) Target Tracking (deadzone + clamp follow)
     2) Target Look Focus
     3) 시점 변경 연출 (Quarter <-> Top)
*/

class CameraController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

public:
    enum class ViewMode
    {
        Quarter   // TODO
    };

    ViewMode currentMode = ViewMode::Quarter;

private:
    Transform* transform = nullptr;       
    Transform* targetTr = nullptr;    

    // offset
    Vector3 quarterOffset = { 0, 280.0f, -230.0f };
    Vector3 quarterEuler = { 50.0f, 0.0f, 0.0f };

    // pivot
    // (campos = pivot + modeOffset)
    Vector3 pivotPos = Vector3::Zero;
    float   groundY = 0.0f;    // pivot.y

    // track values
    float deadRadius = 120.0f;     // target <-> dead zone radius
    float followSpeed = 8.0f;      // following speed

    Vector3 camPosSmooth = Vector3::Zero;
    float camFollowLambda = 10.0f;

public:
    void OnInitialize() override;
    void OnStart() override;
    void OnLateUpdate(float delta) override;
    void OnDestory() override;

    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    static float Clamp(float v, float a, float b) { return std::max(a, std::min(v, b)); }
    static float ToRad(float deg) { return deg * DirectX::XM_PI / 180.0f; }
    static Vector3 DegToRad(const Vector3& deg)
    {
        return Vector3(ToRad(deg.x), ToRad(deg.y), ToRad(deg.z));
    }
    void UpdatePivotByDeadRadius(const Vector3& targetWorldPos, float dt);
};
