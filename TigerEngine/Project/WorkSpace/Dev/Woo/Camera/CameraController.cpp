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

    // desired cam pos init
    Vector3 desiredCamPos = pivotPos + quarterOffset;
    camPosSmooth = desiredCamPos;

    // cam transform init
    Vector3 camPos = pivotPos + quarterOffset;
    transform->SetPosition(camPos);
    transform->SetEuler(DegToRad(quarterEuler));
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
    float t = 1.0f - std::exp(-camFollowLambda * delta);
    camPosSmooth = camPosSmooth + (desiredCamPos - camPosSmooth) * t;

    transform->SetPosition(camPosSmooth);
    transform->SetEuler(DegToRad(quarterEuler));

    // cam transform update
    //Vector3 camPos = pivotPos + quarterOffset;
    //transform->SetPosition(camPos);
    //transform->SetEuler(quarterEuler);
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

    // pivot dead line
    if (distSq <= rSq) return;

    float dist = std::sqrt(distSq);
    Vector3 dir = delta / dist;
    Vector3 desiredPivot = targetGround - dir * r;

    // clamped follow
    pivotPos = desiredPivot;
    pivotPos.y = groundY;
}