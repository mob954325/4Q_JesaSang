#pragma once
#include <PxPhysicsAPI.h>
#include <memory>
#include <directxtk/SimpleMath.h>
#include <unordered_set>

#include "../Util/CollisionLayer.h"
#include "../Object/Component.h"
#include "../Util/DebugDraw.h"
#include "../Base/Datas/EnumData.hpp"

using namespace DirectX::SimpleMath;
using namespace physx;

class Transform;
class CharacterControllerComponent;


// ------------------------------
// Collider Desc
// ------------------------------
struct ColliderDesc
{
    Vector3 halfExtents = { 50,50,50 };// Box
    float radius = 10.0f;            // Sphere / Capsule
    float height = 10.0f;            // Capsule
    float density = 10.0f;           // Dynamic

    Vector3 localOffset = { 0, 0, 0 };
    Quaternion localRotation = Quaternion::Identity; // 회전 아직 적용X 사용하려나..? 

    bool isTrigger = false;
};


// ----------------------------------------------------
// [ [엔진 Transform]  ←→  [PhysX RigidActor] ]
// - 엔진 쪽 Transform (position / rotation) 을 가지고 있는 GameObject(owner) 와 
// - PhysX 쪽 PxRigidActor(m_Actor)
//   이 둘을 양방향으로 동기화 하는 역할 
// ----------------------------------------------------
class PhysicsComponent : public Component
{
    RTTR_ENABLE(Component)
public: 

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

public:
    void OnInitialize() override;
    void OnStart() override;

public:
    Transform* transform = nullptr;

    // Collision / Trigger 이벤트 관리 
    std::unordered_set<PhysicsComponent*> m_CollisionActors;  // 현재 접촉 Actor
    // std::unordered_set<PhysicsComponent*> m_TriggerActors;    // 현재 Trigger Actor
    // std::unordered_set<PhysicsComponent*> m_PendingTriggers; // Trigger 수집용

    bool IsTrigger() const { return m_IsTrigger; }


public:
    // [ 직렬화 대상 ]
    PhysicsBodyType m_BodyType = PhysicsBodyType::Static;
    ColliderType m_ColliderType = ColliderType::Box;

    Vector3 m_HalfExtents = { 10,10,10 };
    float m_Radius = 50.0f;
    float m_Height = 10.0f;
    float m_Density = 10.0f;
    Vector3 m_LocalOffset = { 0,0,0 };

    CollisionLayer m_Layer = CollisionLayer::Default;
    bool m_IsTrigger = false;


    // [ 런타임 전용 ]
    PxRigidActor* m_Actor = nullptr;
    PxShape* m_Shape = nullptr;

    std::unordered_set<PxRigidActor*> m_CCTActors;

public:
    PhysicsComponent() = default;
    ~PhysicsComponent();

    // -----------------------------
    // [ Rigid ]  Collision / Trigger 이벤트 콜백 
    // -----------------------------
    // Rigid <-> Rigid
    virtual void OnCollisionEnter(PhysicsComponent* other);
    virtual void OnCollisionStay(PhysicsComponent* other);
    virtual void OnCollisionExit(PhysicsComponent* other);

    // Trigger <-> Trigger
    virtual void OnTriggerEnter(PhysicsComponent* other);
    virtual void OnTriggerStay(PhysicsComponent* other);
    virtual void OnTriggerExit(PhysicsComponent* other);


    // -----------------------------
    // [ CCT ]  Collision / Trigger 이벤트 콜백 
    // -----------------------------
    // CCT <-> Trigger
    virtual void OnCCTTriggerEnter(CharacterControllerComponent* cct);
    virtual void OnCCTTriggerStay(CharacterControllerComponent* cct);
    virtual void OnCCTTriggerExit(CharacterControllerComponent* cct);

    // CCT <-> Collision
    virtual void OnCCTCollisionEnter(CharacterControllerComponent* cct);
    virtual void OnCCTCollisionStay(CharacterControllerComponent* cct);
    virtual void OnCCTCollisionExit(CharacterControllerComponent* cct);



    // --------------------------
    // 외부 API : Collider / Actor 생성 
    // --------------------------
    void CreateStaticBox(const Vector3& half, const Vector3& localOffset = { 0,0,0 });
    void CreateTriggerBox(const Vector3& half, const Vector3& localOffset = { 0,0,0 }); // Trigger는 Static Actor가 정석
    void CreateDynamicBox(const Vector3& half, float density = 1.0f, const Vector3& localOffset = { 0,0,0 });

    void CreateStaticSphere(float radius, const Vector3& localOffset = { 0,0,0 });
    void CreateDynamicSphere(float radius, float density = 1.0f, const Vector3& localOffset = { 0,0,0 });

    void CreateStaticCapsule(float radius, float height, const Vector3& localOffset = { 0,0,0 });
    void CreateTriggerCapsule(float radius, float height, const Vector3& localOffset = { 0,0,0 });
    void CreateDynamicCapsule(float radius, float height, float density = 1.0f, const Vector3& localOffset = { 0,0,0 });


    // 트리거 체크 
    // void CheckTriggers();
    
    // --------------------------
    // Transform 연동
    // --------------------------
    void SyncToPhysics();
    void SyncFromPhysics();

    // --------------------------
    // 충돌 레이어 
    // --------------------------
    void SetLayer(CollisionLayer layer);
    CollisionLayer GetLayer() const { return m_Layer; }

    // 유틸 
    void DrawPhysXActors();

private:
    void ApplyFilter();

    // 유틸 
    void DrawPhysXShape(PxShape* shape, const PxTransform& actorPose, FXMVECTOR color);
    void DrawCharacterControllers();
    void CollectCCTActors();

    std::unique_ptr<PrimitiveBatch<VertexPositionColor>> m_DebugBatch;

private:
    // --------------------------
    // 내부 생성기
    // --------------------------
    void CreateCollider(ColliderType collider, PhysicsBodyType body, const ColliderDesc& desc);

};
