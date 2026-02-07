#include "VisionComponent.h"
#include "../Util/JsonHelper.h"
#include "../EngineSystem/PhysicsSystem.h"
#include "../Components/Transform.h"
#include "../Object/GameObject.h"
#include "../Util/PhysXUtils.h"
#include "../Util/PhysicsLayerMatrix.h"


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

// [ 타겟이 시야 내에 있는지 확인 ]
bool VisionComponent::CheckVision(GameObject* target, float fov, float maxDistance)
{
    if (!target) return false;

    // 디버그용 값 저장
    m_LastDebug.fov = fov;
    m_LastDebug.dist = maxDistance;
    m_LastDebug.valid = true;


    auto* selfTr = GetOwner()->GetTransform();
    auto* targetTr = target->GetTransform();


    // 시야 원점 & 전방 벡터 
    Vector3 origin = selfTr->GetWorldPosition();
    Vector3 forward = selfTr->GetForward();
    origin.y += eyeHigh; // 눈 위치 보정


    // 타겟까지 방향 & 거리
    Vector3 toTarget = targetTr->GetLocalPosition() - origin;
    float dist = toTarget.Length();
    toTarget.y = origin.y; // FOV 평면 기준!! : 여기도 해줬어야 했음 
    if (dist > maxDistance) return false;

    toTarget.Normalize();


    // FOV 체크 (X-Z 평면 기준)
    Vector3 flatForward = forward; flatForward.y = 0; flatForward.Normalize();
    Vector3 flatToTarget = toTarget; flatToTarget.y = 0; flatToTarget.Normalize();
    float dot = flatForward.Dot(flatToTarget);
    float limit = cosf(Deg2Rad(fov * 0.5f));
    if (dot < limit) return false;


    // PhysX Raycast
    PxVec3 originPx = ToPx(origin);
    PxVec3 dirPx(toTarget.x, toTarget.y, toTarget.z);
    float maxDistPx = maxDistance;

    std::vector<RaycastHit> hits;
    if (!PhysicsSystem::Instance().Raycast(
        originPx,
        dirPx,
        maxDistPx,
        hits,
        CollisionLayer::Default,
        QueryTriggerInteraction::Ignore,
        true)) // 모든 히트 수집
    {
        return false;
    }


    // 거리 순 정렬 (가장 가까운 히트가 front)
    std::sort(hits.begin(), hits.end(),
        [](const RaycastHit& a, const RaycastHit& b) { return a.distance < b.distance; });


    // 첫 번째 유효 히트 찾기 
    PhysicsComponent* firstComp = nullptr;
    for (auto& hit : hits)
    {
        if (!hit.component) continue;

        bool isTrigger = hit.shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE;
        if (isTrigger) continue; // Trigger 무시

        if (!PhysicsLayerMatrix::CanCollide(CollisionLayer::Default, hit.component->GetLayer()))
            continue; // 충돌 가능 레이어 

        // 디버그 
        std::cout << "  Object: " << hit.component->GetOwner()->GetName()
            << " | Layer: " << (uint32_t)hit.component->GetLayer()
            << " | Distance: " << hit.distance
            << std::endl;

        firstComp = hit.component;
        break;
    }

    // 디버그 
    if (firstComp)
    {
        auto* obj = firstComp->GetOwner();
        std::cout << "[Vision] Ray hit -> Object: " << obj->GetName() << " | Layer: " << (uint32_t)firstComp->GetLayer() << std::endl;
    }


    // 타겟과 첫 히트 객체 비교
    return firstComp && firstComp->GetOwner() == target;
}


// [ 시야 디버그 그리기 ]
void VisionComponent::DrawDebugVision()
{
    if (!m_LastDebug.valid) return;

    auto* tr = GetOwner()->GetTransform();
    Vector3 origin = tr->GetWorldPosition();
    origin.y += eyeHigh;               // CheckVision과 동일한 눈 위치 
    Vector3 forward = tr->GetForward();
    Vector3 right = tr->GetRight();

    // PhysX 변환
    PxVec3 originPx = ToPx(origin);
    float maxDistPx = m_LastDebug.dist;

    const int SEGMENTS = 32; // 시야 원 분할 수
    float halfRad = Deg2Rad(m_LastDebug.fov * 0.5f);

    XMVECTOR o = XMVectorSet(origin.x, origin.y + 5.0f, origin.z, 1);

    // FOV 원 시뮬레이션
    for (int i = 0; i <= SEGMENTS; ++i)
    {
        float t = -halfRad + (halfRad * 2.f) * (float(i) / SEGMENTS);

        Vector3 dir = forward * cosf(t) + right * sinf(t);
        dir.y = 0;              // FOV 평면 기준
        dir.Normalize();

        PxVec3 dirPx(dir.x, dir.y, dir.z);

        std::vector<RaycastHit> hits;
        float drawDist = m_LastDebug.dist;

        if (PhysicsSystem::Instance().Raycast(
            originPx,
            dirPx,
            maxDistPx,
            hits,
            CollisionLayer::Default,
            QueryTriggerInteraction::Ignore,
            false // BLOCK 하나
        ))
        {
            // 길이 조절
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