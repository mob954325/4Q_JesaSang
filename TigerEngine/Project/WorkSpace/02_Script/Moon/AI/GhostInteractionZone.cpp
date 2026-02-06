#include "GhostInteractionZone.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"


REGISTER_COMPONENT(GhostInteractionZone)

RTTR_REGISTRATION
{

    rttr::registration::class_<GhostInteractionZone>("GhostInteractionZone")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

nlohmann::json GhostInteractionZone::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void GhostInteractionZone::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}



void GhostInteractionZone::OnStart()
{

}

void GhostInteractionZone::OnFixedUpdate(float delta)
{
    // transform->physics udpate
    auto ob = GetOwner();
    auto tr = ob->GetTransform();
    Vector3 upatePos = tr->GetParent()->GetOwner()->GetTransform()->GetWorldPosition();
    tr->SetPosition(upatePos);
    GetOwner()->GetComponent<PhysicsComponent>()->SyncToPhysics();
}

void GhostInteractionZone::OnTriggerEnter(PhysicsComponent* other)
{
    if (other->GetOwner()->GetName() == "InteractZone")
    {
        cout << "[GhostInteractionSensor] Ghost is OnTriggerEnter From Player's InteractZone" << endl;
    }
}