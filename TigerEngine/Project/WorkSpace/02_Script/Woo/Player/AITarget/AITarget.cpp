#include "AITarget.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"

REGISTER_COMPONENT(AITarget)

RTTR_REGISTRATION
{

    rttr::registration::class_<AITarget>("AITarget")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void AITarget::OnStart()
{
    tr = this->GetOwner()->GetTransform();
    playerTr = this->GetOwner()->GetParent();

    Vector3 upatePos = playerTr->GetWorldPosition() + Vector3(0, 150, 0);
    tr->SetPosition(upatePos);
    GetOwner()->GetComponent<PhysicsComponent>()->SyncToPhysics();
}
void AITarget::OnUpdate(float d)
{

}
void AITarget::OnFixedUpdate(float delta)
{
    Vector3 upatePos = playerTr->GetWorldPosition() + Vector3(0, 150, 0);
    tr->SetPosition(upatePos);
    GetOwner()->GetComponent<PhysicsComponent>()->SyncToPhysics();
}

nlohmann::json AITarget::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void AITarget::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}