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
}
void AITarget::OnFixedUpdate(float delta)
{
    Vector3 upatePos = tr->GetWorldPosition() + Vector3(0, 80, 0);
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