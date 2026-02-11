#include "CameraController.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "Object/GameObject.h"
#include "Components/Camera.h"
#include "EngineSystem/SceneSystem.h"
#include "EngineSystem/CameraSystem.h"


REGISTER_COMPONENT(CameraController)

RTTR_REGISTRATION
{
    rttr::registration::class_<CameraController>("CameraController")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

void CameraController::OnStart()
{
    // set name
    GetOwner()->SetName(camName);
    CameraSystem::Instance().SetCurrCameraByName(camName);

    // get components
    transform = GetOwner()->GetComponent<Transform>();
    targetTr = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("CameraTrackingPoint")->GetComponent<Transform>();

    Vector3 targetPos = targetTr->GetWorldPosition();
    targetPos.z -= 50.0f;

    // pivot pos init
    groundY = targetPos.y;
    pivotPos = { targetPos.x, groundY, targetPos.z };

    // pivot smoothing init
    isTrackingPivot = false;
    pivotVel = Vector3::Zero;

    // mode params init
    Vector3 modeOffset, modeEulerDeg;
    GetModeParams(currentMode, modeOffset, modeEulerDeg);

    // camPos Smooth init
    Vector3 desiredCamPos = pivotPos + modeOffset;
    camPosSmooth = desiredCamPos;

    // look point init
    lookPointSmooth = targetPos;
    lookPointVel = Vector3::Zero;

    // look rotation init
    lookEulerSmooth = DegToRad(modeEulerDeg);
    lookEulerVel = Vector3::Zero;

    // cam transform init
    transform->SetPosition(camPosSmooth);
    transform->SetEuler(DegToRad(modeEulerDeg));   // 처음은 중앙 고정
}

void CameraController::OnFixedUpdate(float delta)
{
    if (!transform || !targetTr)
    {
        cout << "[Camera] Missing Component!" << endl;
        return;
    }

    // target udpate
    const Vector3 targetPos = GetTargetPosWithOffset();

    // pivot udpate
    UpdatePivotByDeadRadius(targetPos, delta);

    // current view mode (offset/euler, look focus)
    Vector3 activeOffset, activeEulerDeg;
    bool useLookFocus = true;
    ResolveViewParams(delta, activeOffset, activeEulerDeg, useLookFocus);

    // camera position update
    UpdateCameraPosition(activeOffset, delta);

    // camera rotation upate
    if (useLookFocus)
        UpdateLookFocus(targetPos, delta);      // Quarter
    else
        ApplyBaseEuler(activeEulerDeg);         // Top, mode change(ing)
}

void CameraController::OnDestory()
{
}

nlohmann::json CameraController::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void CameraController::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

float CameraController::SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime)
{
    smoothTime = std::max(0.0001f, smoothTime);

    float omega = 2.0f / smoothTime;
    float x = omega * deltaTime;

    // 근사 exp
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

    float change = current - target;
    float originalTarget = target;

    // 최대 속도 제한
    float maxChange = maxSpeed * smoothTime;
    change = Clamp(change, -maxChange, maxChange);
    target = current - change;

    float temp = (currentVelocity + omega * change) * deltaTime;
    currentVelocity = (currentVelocity - omega * temp) * exp;

    float output = target + (change + temp) * exp;

    // overshoot 방지
    if ((originalTarget - current > 0.0f) == (output > originalTarget))
    {
        output = originalTarget;
        currentVelocity = (output - originalTarget) / deltaTime;
    }

    return output;
}

Vector3 CameraController::SmoothDampVec3(const Vector3& current, const Vector3& target, Vector3& currentVelocity, float smoothTime, float maxSpeed, float deltaTime)
{
    Vector3 result;
    result.x = SmoothDamp(current.x, target.x, currentVelocity.x, smoothTime, maxSpeed, deltaTime);
    result.y = SmoothDamp(current.y, target.y, currentVelocity.y, smoothTime, maxSpeed, deltaTime);
    result.z = SmoothDamp(current.z, target.z, currentVelocity.z, smoothTime, maxSpeed, deltaTime);
    return result;
}

Vector3 CameraController::ComputeLookEulerRad(const Vector3& camPos, const Vector3& lookTarget)
{
    Vector3 dir = lookTarget - camPos;

    float yaw = std::atan2(dir.x, dir.z);      // z forward 가정
    float xzLen = std::sqrt(dir.x * dir.x + dir.z * dir.z);
    float pitch = -std::atan2(dir.y, xzLen);

    return Vector3(pitch, yaw, 0.0f);
}

Vector3 CameraController::GetTargetPosWithOffset() const
{
    Vector3 p = targetTr->GetWorldPosition();
    p.z -= 50.0f;
    return p;
}

void CameraController::ResolveViewParams(float dt, Vector3& outOffset, Vector3& outEulerDeg, bool& outUseLookFocus)
{
    outUseLookFocus = true;
    UpdateViewBlend(dt, outOffset, outEulerDeg, outUseLookFocus);

    // top/front view일때는 look focus(rotation) off
    if (!isBlendingView && (currentMode == ViewMode::Top || currentMode == ViewMode::Front))
        outUseLookFocus = false;
}

void CameraController::UpdateCameraPosition(const Vector3& activeOffset, float dt)
{
    const Vector3 desiredCamPos = pivotPos + activeOffset;

    const float lambda = isBlendingView ? viewBlendCamLambda : camFollowLambda;
    const float t = 1.0f - std::exp(-lambda * dt);

    camPosSmooth = camPosSmooth + (desiredCamPos - camPosSmooth) * t;

    transform->SetPosition(camPosSmooth);
}

void CameraController::UpdateLookFocus(const Vector3& targetPos, float dt)
{
    // look point target (strength blend)
    const Vector3 targetLookPoint =
        lookPointSmooth +
        (targetPos - lookPointSmooth) * lookPointStrength;

    // look point smoothing
    lookPointSmooth = SmoothDampVec3(
        lookPointSmooth,
        targetLookPoint,
        lookPointVel,
        lookPointSmoothTime,
        lookMaxSpeed,
        dt
    );

    // desired look euler (rad)
    Vector3 desiredLookEuler = ComputeLookEulerRad(camPosSmooth, lookPointSmooth);

    // look dead zone (rad)
    const float deadZoneRad = ToRad(lookDeadZoneDeg);
    if ((desiredLookEuler - lookEulerSmooth).Length() < deadZoneRad)
        desiredLookEuler = lookEulerSmooth;

    // follow strength limit
    const Vector3 limitedLookEuler =
        lookEulerSmooth +
        (desiredLookEuler - lookEulerSmooth) * followStrength;

    // look rotation smoothing
    lookEulerSmooth = SmoothDampVec3(
        lookEulerSmooth,
        limitedLookEuler,
        lookEulerVel,
        lookSmoothTime,
        lookMaxSpeed,
        dt
    );

    transform->SetEuler(lookEulerSmooth);
}

void CameraController::ApplyBaseEuler(const Vector3& activeEulerDeg)
{
    const Vector3 baseEulerRad = DegToRad(activeEulerDeg);

    transform->SetEuler(baseEulerRad);

    // keep internal state synced
    lookEulerSmooth = baseEulerRad;
    lookEulerVel = Vector3::Zero;
}


void CameraController::GetModeParams(ViewMode mode, Vector3& outOffset, Vector3& outEulerDeg) const
{
    switch (mode)
    {
    case ViewMode::Quarter:
        outOffset = quarterOffset;
        outEulerDeg = quarterEuler;
        break;
    case ViewMode::Top:
        outOffset = topOffset;
        outEulerDeg = topEuler;
        break;
    case ViewMode::Front:
        outOffset = frontOffset;
        outEulerDeg = frontEuler;
        break;
    default:
        outOffset = quarterOffset;
        outEulerDeg = quarterEuler;
        break;
    }
}

void CameraController::UpdateViewBlend(float dt, Vector3& outOffset, Vector3& outEulerDeg, bool& outUseLookFocus)
{
    // view 전환중이 아닐때
    if (!isBlendingView)
    {
        GetModeParams(currentMode, outOffset, outEulerDeg);
        outUseLookFocus = true;
        return;
    }

    // 전환 연출중일떄
    viewBlendT += dt / std::max(0.0001f, viewBlendDuration);
    float a = SmoothStep01(viewBlendT); // easing

    Vector3 fromOff, fromEuler, toOff, toEuler;
    GetModeParams(blendFromMode, fromOff, fromEuler);
    GetModeParams(blendToMode, toOff, toEuler);

    outOffset = LerpVec3(fromOff, toOff, a);
    outEulerDeg = LerpVec3(fromEuler, toEuler, a);

    outUseLookFocus = false;    // look focus off (연출용이랑 view mode용이랑 안겹치도록)

    // 전환 연출 완료
    if (viewBlendT >= 1.0f)
    {
        // finish
        isBlendingView = false;
        viewBlendT = 0.0f;
        currentMode = blendToMode;

        // 완료 시점 동기화 (다음 프레임 look focus 켤 때 튐 방지)
        Vector3 finalOff, finalEulerDeg;
        GetModeParams(currentMode, finalOff, finalEulerDeg);
        outOffset = finalOff;
        outEulerDeg = finalEulerDeg;

        lookEulerSmooth = DegToRad(finalEulerDeg);
        lookEulerVel = Vector3::Zero;
        outUseLookFocus = true; // look focus on
    }
}

void CameraController::UpdatePivotByDeadRadius(const Vector3& targetWorldPos, float dt)
{
    // pivot - target dist
    Vector3 targetGround = { targetWorldPos.x, groundY, targetWorldPos.z };
    Vector3 toTarget = targetGround - pivotPos;
    toTarget.y = 0.0f;

    float distSq = toTarget.LengthSquared();
    float dist = std::sqrt(std::max(0.0f, distSq));

    // 경계 부들거림 방지
    if (!isTrackingPivot)
        if (dist > deadHysteresis) isTrackingPivot = true;
    else
        if (dist < deadHysteresis) isTrackingPivot = false;

    // tracking pivot velocity update
    if (!isTrackingPivot)
    {
        float t = 1.0f - std::exp(-12.0f * dt);
        pivotVel = pivotVel + (Vector3::Zero - pivotVel) * t;
        return;
    }

    if (dist < 0.0001f)
        return;

    // direction
    Vector3 dir = toTarget / dist;

    // desiredPivot
    Vector3 desiredPivot = targetGround - dir * deadRadius;
    desiredPivot.y = groundY;

    // pivot pos smoothing
    pivotPos = SmoothDampVec3(pivotPos, desiredPivot, pivotVel, pivotSmoothTime, pivotMaxSpeed, dt);
    pivotPos.y = groundY; // y는 고정
}

void CameraController::SetViewMode(ViewMode mode, bool animate)
{
    if (mode == currentMode && !isBlendingView)
        return;

    if (!animate || viewBlendDuration <= 0.0001f)
    {
        currentMode = mode;
        isBlendingView = false;
        viewBlendT = 0.0f;

        // 즉시 동기화 (튀김 방지)
        Vector3 off, eulerDeg;
        GetModeParams(currentMode, off, eulerDeg);

        camPosSmooth = pivotPos + off;
        transform->SetPosition(camPosSmooth);

        lookEulerSmooth = DegToRad(eulerDeg);
        lookEulerVel = Vector3::Zero;
        transform->SetEuler(lookEulerSmooth);
        return;
    }

    // start blend
    blendFromMode = isBlendingView ? blendToMode : currentMode;
    blendToMode = mode;

    if (blendToMode == ViewMode::Quarter && targetTr)
    {
        Vector3 tp = GetTargetPosWithOffset();
        lookPointSmooth = tp;
        lookPointVel = Vector3::Zero;
        lookEulerSmooth = transform->GetEuler();
        lookEulerVel = Vector3::Zero;
    }

    isBlendingView = true;
    viewBlendT = 0.0f;
}

void CameraController::ToggleViewMode(bool animate)
{
    SetViewMode(currentMode == ViewMode::Quarter ? ViewMode::Top : ViewMode::Quarter, animate);
}

void CameraController::SetTargetTransform(Transform* tr)
{
    if (!tr) return;
    targetTr = tr;
}
