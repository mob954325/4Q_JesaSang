#include "VisionComponent.h"
#include "../Util/JsonHelper.h"
#include "../EngineSystem/PhysicsSystem.h"
#include "../Components/Transform.h"
#include "../Object/GameObject.h"
#include "../Util/PhysXUtils.h"


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
    float fov,
    float maxDistance)
{
    if (!target) return false;

    // 디버그 위한 세팅 저장 
    m_LastDebug.fov = fov;
    m_LastDebug.dist = maxDistance;
    m_LastDebug.valid = true;

    auto* selfTr = GetOwner()->GetTransform();
    auto* targetTr = target->GetTransform();

    Vector3 origin = selfTr->GetWorldPosition();
    origin.y += 100.0f;
    // origin.z += 20.0f;
    Vector3 forward = selfTr->GetForward();

    Vector3 toTarget = targetTr->GetWorldPosition() - origin;
    float dist = toTarget.Length();

    if (dist > maxDistance) return false;
    toTarget.Normalize();


   // FOV 체크
    float dot = forward.Dot(toTarget);
    float limit = cosf(Deg2Rad(fov * 0.5f));
    if (dot < limit)
        return false;


    // PhysX 단위 변환 (이걸 안해주고있었음..) 
    PxVec3 originPx = ToPx(origin);
    PxVec3 dirPx = ToPx(toTarget);

    float maxDistPx = maxDistance * WORLD_TO_PHYSX;

    std::vector<RaycastHit> hits;
    if (!PhysicsSystem::Instance().Raycast(
        originPx,
        dirPx,
        maxDistPx,
        hits,
        CollisionLayer::Default,
        QueryTriggerInteraction::Ignore,
        false)) // 첫 BLOCK만
    {
        return false;
    }


    auto* first = hits.front().component;
    if (first)
    {
        auto* obj = first->GetOwner();
        std::cout << "[Vision] Ray hit -> " << "Object: " << obj->GetName() << " | Layer: " << (uint32_t)first->GetLayer() << std::endl;
    }

    return first && first->GetOwner() == target;
}



void VisionComponent::DrawDebugVision()
{
    if (!m_LastDebug.valid) return;

    auto* tr = GetOwner()->GetTransform();
    Vector3 origin = tr->GetWorldPosition();
    Vector3 forward = tr->GetForward();
    Vector3 right = tr->GetRight();

    const int SEGMENTS = 32;
    float halfRad = Deg2Rad(m_LastDebug.fov * 0.5f);

    XMVECTOR o = XMVectorSet(origin.x, origin.y + 5.0f, origin.z, 1);

    for (int i = 0; i <= SEGMENTS; ++i)
    {
        float t = -halfRad + (halfRad * 2.f) * (float(i) / SEGMENTS);

        Vector3 dir = forward * cosf(t) + right * sinf(t);
        dir.Normalize();

        std::vector<RaycastHit> hits;
        float drawDist = m_LastDebug.dist;

        if (PhysicsSystem::Instance().Raycast(
            PxVec3(origin.x, origin.y, origin.z),
            PxVec3(dir.x, dir.y, dir.z),
            m_LastDebug.dist,
            hits,
            CollisionLayer::World,          
            QueryTriggerInteraction::Ignore,
            false                             // BLOCK 하나
        ))
        {
            drawDist = hits[0].distance;
        }

        XMVECTOR end = o + XMVectorSet(dir.x, dir.y, dir.z, 0) * drawDist;

        DebugDraw::DrawRayDebug(
            DebugDraw::g_Batch.get(),
            o,
            end - o,
            XMVectorSet(0, 1, 0, 1),
            false);
    }
}