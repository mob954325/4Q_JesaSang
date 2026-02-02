#include "PhysicsComponent.h"
#include "../EngineSystem/PhysicsSystem.h"
#include "../EngineSystem/CharacterControllerSystem.h"
#include "../Util/PhysXUtils.h"
#include "../Util/PhysicsLayerMatrix.h"
#include "Transform.h"
#include <DirectXColors.h>
#include "../Object/GameObject.h"
#include "../Util/JsonHelper.h"

RTTR_REGISTRATION
{
    rttr::registration::enumeration<PhysicsBodyType>("PhysicsBodyType")
        (
            rttr::value("Static", PhysicsBodyType::Static),
            rttr::value("Dynamic", PhysicsBodyType::Dynamic),
            rttr::value("Kinematic", PhysicsBodyType::Kinematic)
        );

    rttr::registration::enumeration<ColliderType>("ColliderType")
    (
        rttr::value("Box", ColliderType::Box),
        rttr::value("Sphere", ColliderType::Sphere),
        rttr::value("Capsule", ColliderType::Capsule)
        );

    rttr::registration::enumeration<CollisionLayer>("CollisionLayer")
        (
            rttr::value("Default", CollisionLayer::Default),
            rttr::value("Player", CollisionLayer::Player),
            rttr::value("Enemy", CollisionLayer::Enemy),
            rttr::value("World", CollisionLayer::World),
            rttr::value("Trigger", CollisionLayer::Trigger),
            rttr::value("Projectile", CollisionLayer::Projectile),
            rttr::value("Ball", CollisionLayer::Ball),
            rttr::value("IgnoreTest", CollisionLayer::IgnoreTest)
            );

    rttr::registration::class_<PhysicsComponent>("PhysicsComponent")
        .constructor<>()

        .property("PhysicsBodyType", &PhysicsComponent::m_BodyType)
        .property("ColliderType", &PhysicsComponent::m_ColliderType)

        .property("halfExtents", &PhysicsComponent::m_HalfExtents)
        .property("radius", &PhysicsComponent::m_Radius)
        .property("height", &PhysicsComponent::m_Height)
        .property("density", &PhysicsComponent::m_Density)
        .property("localOffset", &PhysicsComponent::m_LocalOffset)

        .property("layer", &PhysicsComponent::m_Layer)
        .property("isTrigger", &PhysicsComponent::m_IsTrigger);
}

nlohmann::json PhysicsComponent::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void PhysicsComponent::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);

    //// -------------------------
    //// PhysX 재생성
    //// -------------------------
    //ColliderDesc d;
    //d.halfExtents = m_HalfExtents;
    //d.radius = m_Radius;
    //d.height = m_Height;
    //d.density = m_Density;
    //d.localOffset = m_LocalOffset;
    //d.isTrigger = m_IsTrigger;

    //CreateCollider(m_ColliderType, m_BodyType, d); // Note : 반드시 PysicsSystem.Initialize()가 호출된 뒤에 호출되야함.
    //SetLayer(m_Layer);
}

void PhysicsComponent::OnCollisionEnter(PhysicsComponent* other)
{
    if (GetOwner()) GetOwner()->BroadcastCollisionEnter(other);
}

void PhysicsComponent::OnCollisionStay(PhysicsComponent* other)
{
    if (GetOwner()) GetOwner()->BroadcastCollisionStay(other);
}

void PhysicsComponent::OnCollisionExit(PhysicsComponent* other)
{
    if (GetOwner()) GetOwner()->BroadcastCollisionExit(other);
}

void PhysicsComponent::OnTriggerEnter(PhysicsComponent* other)
{
    if (GetOwner()) GetOwner()->BroadcastTriggerEnter(other);
}

void PhysicsComponent::OnTriggerStay(PhysicsComponent* other)
{
    if (GetOwner())GetOwner()->BroadcastTriggerStay(other);
}

void PhysicsComponent::OnTriggerExit(PhysicsComponent* other)
{
    if (GetOwner()) GetOwner()->BroadcastTriggerExit(other);
}

void PhysicsComponent::OnCCTTriggerEnter(CharacterControllerComponent* cct)
{
    if (GetOwner()) GetOwner()->BroadcastCCTTriggerEnter(cct);
}

void PhysicsComponent::OnCCTTriggerStay(CharacterControllerComponent* cct)
{
    if (GetOwner()) GetOwner()->BroadcastCCTTriggerStay(cct);
}

void PhysicsComponent::OnCCTTriggerExit(CharacterControllerComponent* cct)
{
    if (GetOwner()) GetOwner()->BroadcastCCTTriggerExit(cct);
}

void PhysicsComponent::OnCCTCollisionEnter(CharacterControllerComponent* cct)
{
    if (GetOwner()) GetOwner()->BroadcastCCTCollisionEnter(cct);
}

void PhysicsComponent::OnCCTCollisionStay(CharacterControllerComponent* cct)
{
    if (GetOwner()) GetOwner()->BroadcastCCTCollisionStay(cct);
}

void PhysicsComponent::OnCCTCollisionExit(CharacterControllerComponent* cct)
{
    if (GetOwner())  GetOwner()->BroadcastCCTCollisionExit(cct);
}



void PhysicsComponent::OnInitialize()
{
    transform = GetOwner()->GetTransform();

    
}

void PhysicsComponent::OnStart()
{
    // 우정 0128 | PhysicsComponent에서 스스로 collider Create
    ColliderDesc d;
    d.halfExtents = m_HalfExtents;
    d.radius = m_Radius;
    d.height = m_Height;
    d.density = m_Density;
    d.localOffset = m_LocalOffset;
    d.isTrigger = m_IsTrigger;

    CreateCollider(m_ColliderType, m_BodyType, d);
    SetLayer(m_Layer);
}

PhysicsComponent::~PhysicsComponent()
{
    PhysicsSystem::Instance().UnregisterComponent(this);
}


// ------------------------------
// 좌표 변환
// ------------------------------

// Transform → Physics  : Dynamic에는 매 프레임 쓰면 안 됨
void PhysicsComponent::SyncToPhysics()
{
    if (!m_Actor || !transform) return;

    PxTransform px;
    px.p = ToPx(transform->GetLocalPosition());
    px.q = ToPxQuat(transform->GetQuaternion()); 

    m_Actor->setGlobalPose(px);
}

// Physics → Transform : 물리 시뮬 하고나서 매 프레임 실행
void PhysicsComponent::SyncFromPhysics()
{
    if (!transform) return;

    if (m_Actor) // 일반 액터 
    {
        PxTransform px = m_Actor->getGlobalPose();
        transform->SetPosition(ToDX(px.p));  
        transform->SetQuaternion(ToDXQuat(px.q)); 
    }
}

void PhysicsComponent::ApplyFilter()
{
    // ----------------------------
    // RigidActor / Shape 용
    // ----------------------------
    if (m_Shape)
    {
        PxFilterData data;
        data.word0 = (uint32_t)m_Layer;
        data.word1 = PhysicsLayerMatrix::GetMask(m_Layer);
        data.word2 = 0;
        data.word3 = 0;

        m_Shape->setSimulationFilterData(data);
        m_Shape->setQueryFilterData(data);
    }
}

void PhysicsComponent::SetLayer(CollisionLayer layer)
{
    m_Layer = layer;
    ApplyFilter();
}



// ------------------------------
// 외부 API 
// - density : 질량 
// ------------------------------

// Box 
void PhysicsComponent::CreateStaticBox(const Vector3& half, const Vector3& localOffset)
{
    ColliderDesc d;
    d.halfExtents = half;
    d.localOffset = localOffset;
    CreateCollider(ColliderType::Box, PhysicsBodyType::Static, d);
}
void PhysicsComponent::CreateTriggerBox(const Vector3& half, const Vector3& localOffset)
{
    ColliderDesc d;
    d.halfExtents = half;
    d.localOffset = localOffset;
    d.isTrigger = true;

    CreateCollider(ColliderType::Box, PhysicsBodyType::Static, d);
}
void PhysicsComponent::CreateDynamicBox(const Vector3& half, float density, const Vector3& localOffset)
{
    ColliderDesc d;
    d.halfExtents = half;
    d.density = density;
    d.localOffset = localOffset;
    CreateCollider(ColliderType::Box, PhysicsBodyType::Dynamic, d);
}


// Sphere 
void PhysicsComponent::CreateStaticSphere(float radius, const Vector3& localOffset)
{
    ColliderDesc d;
    d.radius = radius;
    d.localOffset = localOffset;
    CreateCollider(ColliderType::Sphere, PhysicsBodyType::Static, d);
}
void PhysicsComponent::CreateDynamicSphere(float radius, float density, const Vector3& localOffset)
{
    ColliderDesc d;
    d.radius = radius;
    d.density = density;
    d.localOffset = localOffset;
    CreateCollider(ColliderType::Sphere, PhysicsBodyType::Dynamic, d);
}

// Capsule 
void PhysicsComponent::CreateStaticCapsule(float radius, float height, const Vector3& localOffset)
{
    ColliderDesc d;
    d.radius = radius;
    d.height = height;
    d.localOffset = localOffset;
    CreateCollider(ColliderType::Capsule, PhysicsBodyType::Static, d);
}
void PhysicsComponent::CreateTriggerCapsule(float radius, float height, const Vector3& localOffset)
{
    ColliderDesc d;
    d.radius = radius;
    d.height = height;
    d.localOffset = localOffset;
    d.isTrigger = true;

    CreateCollider(ColliderType::Capsule, PhysicsBodyType::Static, d);
}
void PhysicsComponent::CreateDynamicCapsule(float radius, float height, float density, const Vector3& localOffset)
{
    ColliderDesc d;
    d.radius = radius;
    d.height = height;
    d.density = density;
    d.localOffset = localOffset;
    CreateCollider(ColliderType::Capsule, PhysicsBodyType::Dynamic, d);
}


// ------------------------------
// 내부 생성 
// ------------------------------
void PhysicsComponent::CreateCollider(ColliderType collider, PhysicsBodyType body, const ColliderDesc& d)
{
    auto& phys = PhysicsSystem::Instance();
    PxPhysics* px = phys.GetPhysics();
    PxMaterial* mat = phys.GetDefaultMaterial();

    m_IsTrigger = d.isTrigger;

    // ----------------------
    // Shape 생성
    // ----------------------
    PxTransform localPose;
    localPose.p = ToPx(d.localOffset);
    localPose.q = ToPxQuat(XMLoadFloat4(&d.localRotation));

    switch (collider)
    {
    case ColliderType::Box:
        m_Shape = px->createShape(PxBoxGeometry(d.halfExtents.x * WORLD_TO_PHYSX, d.halfExtents.y * WORLD_TO_PHYSX, d.halfExtents.z * WORLD_TO_PHYSX), *mat, true);
        break;

    case ColliderType::Sphere:
        m_Shape = px->createShape(PxSphereGeometry(d.radius * WORLD_TO_PHYSX), *mat, true);
        break;

    case ColliderType::Capsule:
        m_Shape = px->createShape(PxCapsuleGeometry(d.radius * WORLD_TO_PHYSX, (d.height * 0.5f) * WORLD_TO_PHYSX), *mat, true);

        PxQuat capsuleRot(PxHalfPi, PxVec3(0, 0, 1));// X축 캡슐 → Y축 캡슐로 회전 // Z축 +90도
        localPose.q = capsuleRot * localPose.q;
        break;
    }
    m_Shape->setLocalPose(localPose);


    // ----------------------
    // Shape Flag (Trigger / Simulation)
    // ----------------------
    if (d.isTrigger)
    {
        // Trigger는 충돌 계산 X
        m_Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
        m_Shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
    }
    else
    {
        // 일반 Collider
        m_Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        m_Shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
    }


    // ----------------------
    // Actor 생성
    // ----------------------
    if (body == PhysicsBodyType::Static || d.isTrigger)
    {
        // Trigger는 무조건 Static 
        m_Actor = px->createRigidStatic(PxTransform(PxIdentity));
    }
    else
    {
        PxRigidDynamic* dyn = px->createRigidDynamic(PxTransform(PxIdentity));

        if (body == PhysicsBodyType::Kinematic)
        {
            dyn->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
        }

        m_Actor = dyn;
    }

    // ----------------------
    // Shape 연결
    // ----------------------
    m_Actor->attachShape(*m_Shape);


    // ----------------------
    // 질량 계산
    // ----------------------
    if (body == PhysicsBodyType::Dynamic)
    {
        PxRigidBodyExt::updateMassAndInertia( // 질량 계산 : Shape 부피 × density
            *static_cast<PxRigidDynamic*>(m_Actor),
            d.density
        );
    }
    
    phys.GetScene()->addActor(*m_Actor); // 물리 씬에 추가 
    // phys.RegisterComponent(m_Actor, this);
    phys.RegisterComponent(this, m_Actor);


    SyncToPhysics(); // 좌표 연결 

    m_BodyType = body;
    m_ColliderType = collider;

    ApplyFilter(); // 레이어 필터 
}


void PhysicsComponent::CheckTriggers()
{
    if (!m_Actor) return; // PxActor* m_Actor; (PhysicsSystem에서 매핑됨)

    PxScene* scene = PhysicsSystem::Instance().GetScene();

    // Actor에 연결된 모든 Shape 가져오기
    PxU32 shapeCount = m_Actor->getNbShapes();
    if (shapeCount == 0) return;

    std::vector<PxShape*> shapes(shapeCount);
    m_Actor->getShapes(shapes.data(), shapeCount);

    for (PxShape* shape : shapes)
    {
        // Trigger Shape만 처리
        if (!(shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE))
            continue;

        PxGeometryHolder geom = shape->getGeometry();
        PxTransform pose = m_Actor->getGlobalPose() * shape->getLocalPose();

        // Overlap Query 수행
        PxOverlapBufferN<64> hitBuffer;
        PxFilterData shapeFilter = shape->getQueryFilterData();
        PxQueryFilterData filterData;
        filterData.data = shapeFilter; 
        filterData.flags =
            PxQueryFlag::eSTATIC |
            PxQueryFlag::eDYNAMIC |
            PxQueryFlag::ePREFILTER;

        if (scene->overlap(geom.any(), pose, hitBuffer, filterData))
        {
            for (PxU32 i = 0; i < hitBuffer.getNbAnyHits(); ++i)
            {
                PxRigidActor* otherActor = hitBuffer.getAnyHit(i).actor;
                if (!otherActor) continue;

                PhysicsComponent* other = PhysicsSystem::Instance().GetComponent(otherActor);
                if (!other || other == this)
                    continue;

                // Trigger <-> Trigger 중복 방지
                if (this < other)
                    m_PendingTriggers.insert(other);
            }
        }
    }
}


DirectX::XMVECTOR GetActorDebugColor(PxRigidActor* actor)
{
    if (actor->is<PxRigidStatic>())
        return DirectX::Colors::Green;

    if (PxRigidDynamic* dyn = actor->is<PxRigidDynamic>())
    {
        if (dyn->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
            return DirectX::Colors::Magenta;

        return DirectX::Colors::Cyan;
    }

    return DirectX::Colors::White;
}


void PhysicsComponent::DrawPhysXActors()
{
    PxScene* scene = PhysicsSystem::Instance().GetScene();
    if (!scene) return;

    PxU32 actorCount = scene->getNbActors(
        PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC
    );

    std::vector<PxActor*> actors(actorCount);
    scene->getActors(
        PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC,
        actors.data(),
        actorCount
    );

    // [ Actor ] 
    for (PxActor* actor : actors)
    {
        PxRigidActor* rigid = actor->is<PxRigidActor>();
        if (!rigid) return;

        // CCT가 소유한 Actor는 제외 
        if (m_CCTActors.find(rigid) != m_CCTActors.end()) // if (m_CCTActors.contains(rigid)) 없음 
            continue;

        const XMVECTOR debugColor = GetActorDebugColor(rigid);
        PxTransform actorPose = rigid->getGlobalPose();

        PxU32 shapeCount = rigid->getNbShapes();
        std::vector<PxShape*> shapes(shapeCount);
        rigid->getShapes(shapes.data(), shapeCount);

        for (PxShape* shape : shapes)
        {
            // [ Shape ]
            DrawPhysXShape(shape, actorPose, debugColor);
        }
    }

    // Character Controller는 별도로 그려야 함 
    DrawCharacterControllers();
}


void PhysicsComponent::DrawPhysXShape(PxShape* shape, const PxTransform& actorPose, FXMVECTOR color)
{
    bool isTrigger = shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE;

    PxGeometryHolder geo = shape->getGeometry();
    PxTransform localPose = shape->getLocalPose();
    PxTransform worldPose = actorPose * localPose; // Actor * Shape Local

    switch (geo.getType())
    {
        // BOX
    case PxGeometryType::eBOX:
    {
        const PxBoxGeometry& box = geo.box();

        DirectX::BoundingOrientedBox obb;
        obb.Center = ToDX(worldPose.p);   // m -> cm
        obb.Extents = { PxToDX(box.halfExtents.x),PxToDX(box.halfExtents.y),PxToDX(box.halfExtents.z) };
        obb.Orientation = { worldPose.q.x,worldPose.q.y,worldPose.q.z,worldPose.q.w };

        DebugDraw::Draw(DebugDraw::g_Batch.get(), obb, color, isTrigger);
        break;
    }

    // SPHERE
    case PxGeometryType::eSPHERE:
    {
        const PxSphereGeometry& sphere = geo.sphere();

        DirectX::BoundingSphere bs;
        bs.Center = ToDX(worldPose.p);
        bs.Radius = PxToDX(sphere.radius);

        DebugDraw::Draw(DebugDraw::g_Batch.get(), bs, color, isTrigger);
        break;
    }

    // CAPSULE (PhysX X축 -> Render Y축)
    case PxGeometryType::eCAPSULE:
    {
        const PxCapsuleGeometry& capsule = geo.capsule();

        // PhysX 캡슐(X축)을 Render(Y축)으로 보정
        XMVECTOR physxToY = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), XM_PIDIV2);

        XMVECTOR worldQ =
            XMVectorSet(worldPose.q.x, worldPose.q.y, worldPose.q.z, worldPose.q.w);

        XMVECTOR debugQ =
            XMQuaternionMultiply(physxToY, worldQ);

        PxQuat finalQ = ToPxQuat(debugQ);

        DebugDraw::DrawCapsule(
            DebugDraw::g_Batch.get(),
            PxVec3(
                worldPose.p.x * PHYSX_TO_WORLD,
                worldPose.p.y * PHYSX_TO_WORLD,
                worldPose.p.z * PHYSX_TO_WORLD
            ),
            capsule.radius * PHYSX_TO_WORLD,
            (capsule.halfHeight * 2.0f) * PHYSX_TO_WORLD,
            color,
            finalQ,
            isTrigger
        );
        break;
    }
    default:
        break;
    }
}


void PhysicsComponent::DrawCharacterControllers()
{
    PxControllerManager* mgr = CharacterControllerSystem::Instance().GetControllerManager();
    if (!mgr) return;

    PxU32 count = mgr->getNbControllers();

    for (PxU32 i = 0; i < count; ++i)
    {
        PxController* cct = mgr->getController(i);
        if (!cct) continue;

        if (cct->getType() != PxControllerShapeType::eCAPSULE)
            continue;

        PxCapsuleController* capsule = static_cast<PxCapsuleController*>(cct);

        PxExtendedVec3 p = capsule->getPosition();

        DebugDraw::DrawCapsule(
            DebugDraw::g_Batch.get(),
            PxVec3(
                (float)p.x * PHYSX_TO_WORLD,
                (float)p.y * PHYSX_TO_WORLD,
                (float)p.z * PHYSX_TO_WORLD
            ),
            capsule->getRadius() * PHYSX_TO_WORLD,
            capsule->getHeight() * PHYSX_TO_WORLD,
            DirectX::Colors::Red
        );
    }
}


// [ CCT Actor 수집 함수 ]
void PhysicsComponent::CollectCCTActors()
{
    m_CCTActors.clear();

    PxControllerManager* mgr = CharacterControllerSystem::Instance().GetControllerManager();
    if (!mgr) return;

    PxU32 count = mgr->getNbControllers();
    for (PxU32 i = 0; i < count; ++i)
    {
        PxController* cct = mgr->getController(i);
        if (!cct) continue;

        if (PxRigidActor* actor = cct->getActor())
        {
            m_CCTActors.insert(actor);
        }
    }
}


