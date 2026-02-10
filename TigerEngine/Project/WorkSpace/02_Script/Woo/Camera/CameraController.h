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
        Quarter, Top, Front
    };

    ViewMode currentMode = ViewMode::Quarter;

private:
    // [ components ]
    Transform* transform = nullptr;       
    Transform* targetTr = nullptr;   


    // [ data ]
    // offset
    Vector3 quarterOffset = { 0, 240.0f, -200.0f };
    Vector3 quarterEuler = { 50.0f, 0.0f, 0.0f };

    Vector3 topOffset = { 0, 550.0f, -20.0f };
    Vector3 topEuler = { 82.0f, 0.0f, 0.0f };

    Vector3 frontOffset = { 0, 50.0f, -110.0f };
    Vector3 frontEuler = { 10.0f, 0.0f, 0.0f };

    // view transition
    float viewBlendDuration = 0.65f;     // Quarter <-> Top 전환 시간
    float viewBlendCamLambda = 10.0f;    // 전환 중 위치 반응(크면 더 빠르게 따라감)

    // dead radius
    float deadRadius      = 60.0f;   // target <-> dead zone radius
    float deadHysteresis  = 10.0f;   // 경계 떨림 방지

    // tracking smooth
    float   pivotSmoothTime = 0.22f;
    float   camFollowLambda = 6.0f;

    // look focus smooth
    float lookPointSmoothTime = 0.4f;     // target pos -> look point smoothing time
    float lookSmoothTime      = 0.15f;    // look euler smoothing time
    float lookMaxSpeed        = 1000.0f;  // look euler max speed
    float lookDeadZoneDeg     = 1.5f;     // 회전 무시 threshold
                                          
    // strength                           
    float lookPointStrength = 0.6f;       // look point 영향 강도
    float followStrength    = 0.2f;       // look eule 영향 강도 


    // [ controll ]
    // tracking & looking
    bool    isTrackingPivot  = false;             // tracking state flag
    Vector3 pivotPos         = Vector3::Zero;     // pivot (campos = pivot + modeOffset)
    float   groundY          = 0.0f;              // pivot.y
    Vector3 pivotVel         = Vector3::Zero;
    float   pivotMaxSpeed    = 5000.0f;
    Vector3 camPosSmooth     = Vector3::Zero;
    Vector3 lookPointSmooth  = Vector3::Zero;     // look point (target pos smoothing)
    Vector3 lookPointVel     = Vector3::Zero;     // look point smooth velocity
    Vector3 lookEulerSmooth  = Vector3::Zero;     // rotation euler
    Vector3 lookEulerVel     = Vector3::Zero;     // rotation euler smooth velocity

    // view transition state
    bool     isBlendingView = false;
    float    viewBlendT = 0.0f;
    ViewMode blendFromMode = ViewMode::Quarter;
    ViewMode blendToMode = ViewMode::Quarter;


public:
    void OnStart() override;
    void OnFixedUpdate(float delta) override;
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
    static float SmoothStep01(float t)
    {
        t = Clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
    static Vector3 LerpVec3(const Vector3& a, const Vector3& b, float t)
    {
        return a + (b - a) * t;
    }
    static float SmoothDamp(float current, float target, float& currentVelocity,
        float smoothTime, float maxSpeed, float deltaTime);
    static Vector3 SmoothDampVec3(const Vector3& current, const Vector3& target, Vector3& currentVelocity,
        float smoothTime, float maxSpeed, float deltaTime);
    static Vector3 ComputeLookEulerRad(const Vector3& camPos, const Vector3& lookTarget);

    // funs..
    Vector3 GetTargetPosWithOffset() const;
    void UpdatePivotByDeadRadius(const Vector3& targetWorldPos, float dt);
    void UpdateCameraPosition(const Vector3& activeOffset, float dt);
    void UpdateLookFocus(const Vector3& targetPos, float dt);
    void ApplyBaseEuler(const Vector3& activeEulerDeg);

    // view mode
    void GetModeParams(ViewMode mode, Vector3& outOffset, Vector3& outEulerDeg) const;
    void ResolveViewParams(float dt, Vector3& outOffset, Vector3& outEulerDeg, bool& outUseLookFocus);
    void UpdateViewBlend(float dt, Vector3& outOffset, Vector3& outEulerDeg, bool& outUseLookFocus);

public:
    // 외부 call funcs..
    void SetViewMode(ViewMode mode, bool animate = true);
    void ToggleViewMode(bool animate = true);

    void SetTargetTransform(Transform* tr);
};
