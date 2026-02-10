#include "Weapon.h"
#include "Object/GameObject.h"
#include "System/InputSystem.h"
#include "Object/Component.h"
#include "Components/FBXRenderer.h"
#include "Util/ComponentAutoRegister.h"
#include "../Engine/EngineSystem/SceneSystem.h"
#include "Util/PrefabUtil.h"
#include "../Engine//EngineSystem/CameraSystem.h"
#include "../Engine/Util/JsonHelper.h"

REGISTER_COMPONENT(Weapon);

RTTR_REGISTRATION
{
    rttr::registration::class_<Weapon>("Weapon")
    .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

void Weapon::OnInitialize()
{  
}

void Weapon::OnEnable()
{

}

void Weapon::OnStart()
{
    ess = GetOwner()->GetComponent<EnemySoundSource>();
}

void Weapon::OnDisable()
{
}

void Weapon::OnDestory()
{
}

void Weapon::OnUpdate(float delta)
{
    // 매 프레임: Transform(Vector3 등) -> AudioTransform(XMFLOAT3)로 변환해서 fallback에 주입

    //if (!listener) return;

    //auto pos = GetOwner()->GetTransform()->GetWorldPosition();
    //auto fwd = GetOwner()->GetTransform()->GetForward();
    //auto up = GetOwner()->GetTransform()->GetUp();
    //Vector3 vel = Vector3::Zero;
    //if (hasPrev && delta > 0.0001f)
    //    vel = (pos - prevPos) / delta * 0.01f;

    //prevPos = pos;
    //hasPrev = true;

    //AudioTransform t{};
    //t.position = { pos.x, pos.y, pos.z };       // 위치
    //t.forward = { fwd.x, fwd.y, fwd.z };        // forward
    //t.up = { up.x,  up.y,  up.z };              // up vector
    //t.velocity = { vel.x, vel.y, vel.z };       // "초당 이동량"(world-space). doppler 등에 사용됨. ( 청자(Listener)가 얼마나 / 어느 방향으로 움직이는지 알려주는 값 

    //listener->SetFallback(t); // AudioListenerComponent가 Update()에서 적용(Engine/Components/AudioListenerComponent.cpp:61)

    if (ess)ess->UpdateAudioTransform();
    timer += delta;
    if (timer > 1.0f)
    {
        timer = 0;
        ess->PlaySound(EnemySoundType::Ghost_Move_Sound);
    }
}

nlohmann::json Weapon::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void Weapon::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}
