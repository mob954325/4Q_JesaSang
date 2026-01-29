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
    float deadHysteresis = 10.0f;  // 경계 떨림 방지

    // smooth
    bool   isTrackingPivot = false;  // hysteresis 상태
    Vector3 pivotVel = Vector3::Zero;
    float  pivotSmoothTime = 0.22f;  // 값 키우면 더 묵직/느리게 따라옴
    float  pivotMaxSpeed = 5000.0f;

    // camera smoothing (기존)
    Vector3 camPosSmooth = Vector3::Zero;
    float   camFollowLambda = 6.0f;  // 기존 10 -> 6 (조금 더 부드럽게)

public:
    void OnInitialize() override;
    void OnStart() override;
    void OnLateUpdate(float delta) override;
    void OnDestory() override;

    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    // utils
    static float Clamp(float v, float a, float b) { return std::max(a, std::min(v, b)); }
    static float ToRad(float deg) { return deg * DirectX::XM_PI / 180.0f; }
    static Vector3 DegToRad(const Vector3& deg)
    {
        return Vector3(ToRad(deg.x), ToRad(deg.y), ToRad(deg.z));
    }
    static float SmoothDamp(float current, float target, float& currentVelocity,
        float smoothTime, float maxSpeed, float deltaTime);

    static Vector3 SmoothDampVec3(const Vector3& current, const Vector3& target, Vector3& currentVelocity,
        float smoothTime, float maxSpeed, float deltaTime);


    // funs..
    void UpdatePivotByDeadRadius(const Vector3& targetWorldPos, float dt);
};
