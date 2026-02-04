#include "VisionComponent.h"
#include "../Util/JsonHelper.h"
#include "../EngineSystem/PhysicsSystem.h"
#include "../Components/Transform.h"
#include "../Object/GameObject.h"


static float Deg2Rad(float deg)
{
    return deg * 3.1415926535f / 180.0f;
}

RTTR_REGISTRATION
{
    rttr::registration::class_<VisionComponent>("VisionComponent")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}
nlohmann::json VisionComponent::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}
void VisionComponent::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void VisionComponent::Enable_Inner()
{
    ScriptSystem::Instance().Register(this);
}

void VisionComponent::Disable_Inner()
{
    ScriptSystem::Instance().UnRegister(this);
}

// -----------------------------------------------------------

bool VisionComponent::CheckVision(
    GameObject* target,
    float fovDeg,
    float viewDistance,
    CollisionMask targetMask,
    CollisionMask occlusionMask)
{
    if (!target) return false;

    auto* selfTr = GetOwner()->GetTransform();
    auto* targetTr = target->GetTransform();

    Vector3 origin = selfTr->GetWorldPosition();
    Vector3 forward = selfTr->GetForward();

    Vector3 toTarget = targetTr->GetWorldPosition() - origin;
    float dist = toTarget.Length();

    // 거리 제한
    if (dist > viewDistance)
        return false;

    toTarget.Normalize();

    // FOV 체크
    float dot = forward.Dot(toTarget);
    float limit = cosf(Deg2Rad(fovDeg * 0.5f));
    if (dot < limit)
        return false;

    // ---------- 가림막 Raycast ----------
    std::vector<RaycastHit> hits;

    bool hit = PhysicsSystem::Instance().Raycast(
        PxVec3(origin.x, origin.y, origin.z),
        PxVec3(toTarget.x, toTarget.y, toTarget.z),
        dist,
        hits,
        occlusionMask,
        QueryTriggerInteraction::Ignore,
        false // 첫 BLOCK만
    );

    if (hit && !hits.empty())
    {
        // 가장 가까운 물체가 target이 아니면 가림
        auto* hitComp = hits[0].component;
        if (!hitComp || hitComp->GetOwner() != target)
            return false;
    }

    return true;
}