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
}

void CameraController::OnStart()
{
    transform = GetOwner()->GetComponent<Transform>();
    targetTr = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("CameraTrackingPoint")->GetComponent<Transform>();

    // pivot pos init
    Vector3 tpos = targetTr->GetWorldPosition();
    groundY = tpos.y;
    pivotPos = { tpos.x, groundY, tpos.z };

    // cam transform init
    Vector3 camPos = pivotPos + quarterOffset;
    transform->SetPosition(camPos);
    transform->SetEuler(quarterEuler);
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
    targetPos.z -= 200.0f;

    // pivot pos update
    UpdatePivotByDeadRadius(targetPos, delta);

    // cam transform update
    Vector3 camPos = pivotPos + quarterOffset;
    transform->SetPosition(camPos);
    transform->SetEuler(quarterEuler);
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

void CameraController::UpdatePivotByDeadRadius(const Vector3& targetWorldPos, float dt)
{
    // target을 ground 평면에 투영
    Vector3 targetGround = { targetWorldPos.x, groundY, targetWorldPos.z };

    Vector3 delta = targetGround - pivotPos;
    delta.y = 0.0f;

    float distSq = delta.LengthSquared();
    float r = std::max(0.0f, deadRadius);
    float rSq = r * r;

    // dead zone 안 : pivot 유지
    if (distSq <= rSq)
        return;

    float dist = std::sqrt(distSq);
    Vector3 dir = delta / dist; // normalize

    // 경계까지만 따라간 pivot 목표
    Vector3 desiredPivot = targetGround - dir * r;

    // 부드럽게 따라가기
    float t = 1.0f - std::exp(-followLambda * dt);
    pivotPos = pivotPos + (desiredPivot - pivotPos) * t;
    pivotPos.y = groundY;
}