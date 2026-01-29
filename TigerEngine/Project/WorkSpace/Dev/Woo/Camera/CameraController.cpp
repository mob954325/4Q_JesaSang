#include "CameraController.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "Object/GameObject.h"
#include "Components/Camera.h"
#include "EngineSystem/SceneSystem.h"


REGISTER_COMPONENT(CameraController)

RTTR_REGISTRATION
{
    rttr::registration::class_<CameraController>("CameraController")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

void CameraController::OnInitialize()
{
    transform = GetOwner()->GetComponent<Transform>();
    targetTr = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("CameraTrackingPoint")->GetComponent<Transform>();
}

void CameraController::OnStart()
{
    if(!transform)
        transform = GetOwner()->GetComponent<Transform>();
    if (!targetTr)
        targetTr = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("CameraTrackingPoint")->GetComponent<Transform>();

    // pivot pos init
    Vector3 tpos = targetTr->GetWorldPosition();
    groundY = tpos.y;
    pivotPos = { tpos.x, groundY, tpos.z };

    // pivot smoothing init
    isTrackingPivot = false;
    pivotVel = Vector3::Zero;

    // desired cam pos init
    Vector3 desiredCamPos = pivotPos + quarterOffset;
    camPosSmooth = desiredCamPos;

    // cam transform init
    transform->SetPosition(camPosSmooth);
    transform->SetEuler(DegToRad(quarterEuler));

    // look init
    lookEulerSmooth = DegToRad(quarterEuler);
    lookEulerVel = Vector3::Zero;
}

void CameraController::OnLateUpdate(float delta)
{
    if (!transform || !targetTr)
    {
        cout << "[Camera] Missing Component!" << endl;
        return;
    }

    // target pos update
    Vector3 targetPos = targetTr->GetWorldPosition();
    targetPos.z -= 50.0f;

    // pivot pos update
    UpdatePivotByDeadRadius(targetPos, delta);

    // desired cam pos update
    Vector3 desiredCamPos = pivotPos + quarterOffset;

    // cam smoothing
    float t = 1.0f - std::exp(-camFollowLambda * delta);
    camPosSmooth = camPosSmooth + (desiredCamPos - camPosSmooth) * t;

    // apply position
    transform->SetPosition(camPosSmooth);

    // apply rotation
    if (true)
    {
        // dead zone 안: look focus
        Vector3 lookPoint = targetPos + lookAtOffset;
        Vector3 desiredLookEuler = ComputeLookEulerRad(camPosSmooth, lookPoint);

        // SmoothDampVec3로 회전 스무딩 (rad 단위)
        lookEulerSmooth = SmoothDampVec3(
            lookEulerSmooth,
            desiredLookEuler,
            lookEulerVel,
            lookSmoothTime,
            lookMaxSpeed,
            delta
        );

        transform->SetEuler(lookEulerSmooth);
    }
    else
    {
        // dead zone 밖(또는 비활성): 기존 고정 euler
        lookEulerSmooth = DegToRad(quarterEuler); // 상태 전환 시 튐 방지용 동기화
        lookEulerVel = Vector3::Zero;
        transform->SetEuler(lookEulerSmooth);
    }
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

    // 근사 exp (Unity 방식)
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

    float change = current - target;
    float originalTarget = target;

    // 최대 속도 제한 (1프레임에 너무 멀리 못 가게)
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

void CameraController::UpdatePivotByDeadRadius(const Vector3& targetWorldPos, float dt)
{
    // target을 ground 평면에 투영
    Vector3 targetGround = { targetWorldPos.x, groundY, targetWorldPos.z };

    Vector3 toTarget = targetGround - pivotPos;
    toTarget.y = 0.0f;

    float distSq = toTarget.LengthSquared();
    float dist = std::sqrt(std::max(0.0f, distSq));

    float r = std::max(0.0f, deadRadius);
    float enterR = r + std::max(0.0f, deadHysteresis); // 밖으로 나가면 tracking 시작
    float exitR = std::max(0.0f, r - std::max(0.0f, deadHysteresis)); // 안으로 충분히 들어오면 tracking 종료

    // hysteresis 상태 전환 (경계 부들거림 방지)
    if (!isTrackingPivot)
    {
        if (dist > enterR) isTrackingPivot = true;
    }
    else
    {
        if (dist < exitR) isTrackingPivot = false;
    }

    // tracking 안 하면 pivot 고정(속도만 서서히 줄여서 잔떨림 제거)
    if (!isTrackingPivot)
    {
        // 약간의 damping으로 velocity를 죽여줌 (갑자기 0으로 만들면 또 흔들릴 수 있음)
        float t = 1.0f - std::exp(-12.0f * dt);
        pivotVel = pivotVel + (Vector3::Zero - pivotVel) * t;
        return;
    }

    // dist가 거의 0이면 dir 계산 불안정 -> 방어
    if (dist < 0.0001f)
        return;

    Vector3 dir = toTarget / dist;

    // desiredPivot: 타겟이 deadRadius 바깥으로 나간 만큼 pivot이 따라가되, "deadRadius 경계에 걸치도록"
    Vector3 desiredPivot = targetGround - dir * r;
    desiredPivot.y = groundY;

    // 핵심: pivot을 desiredPivot으로 SmoothDamp
    pivotPos = SmoothDampVec3(pivotPos, desiredPivot, pivotVel, pivotSmoothTime, pivotMaxSpeed, dt);
    pivotPos.y = groundY; // y는 고정
}