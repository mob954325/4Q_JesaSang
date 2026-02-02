#include "InteractionZone.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"
#include "../../Object/SearchObject.h"
#include "EngineSystem/PhysicsSystem.h"

REGISTER_COMPONENT(InteractionZone)

RTTR_REGISTRATION
{

    rttr::registration::class_<InteractionZone>("InteractionZone")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void InteractionZone::OnStart()
{
    player = this->GetOwner()->GetTransform()->GetParent()->GetOwner()->GetComponent<PlayerController>();
    if (!player)
        cout << "[InteractionSensor] player component missing!" << endl;
}

void InteractionZone::OnUpdate(float delta)
{
    
}

void InteractionZone::OnFixedUpdate(float delta)
{
    // transform->physics udpate
    auto ob = GetOwner();
    auto tr = ob->GetTransform();
    tr->SetPosition(tr->GetParent()->GetOwner()->GetTransform()->GetWorldPosition());
    GetOwner()->GetComponent<PhysicsComponent>()->SyncToPhysics();
}

nlohmann::json InteractionZone::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void InteractionZone::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void InteractionZone::OnTriggerEnter(PhysicsComponent* other)
{
    cout << "InteractionZone : " << other->GetName() << endl;
    if (other->GetOwner()->GetName() == "SearchOB_Item")
    {
        player->SetCurSearchObject(other->GetOwner()->GetComponent<SearchObject>());
        // TODO :: UI
    }
}

void InteractionZone::OnTriggerStay(PhysicsComponent* other)
{
    cout << "InteractionSensor : " << other->GetName() << endl;
}

void InteractionZone::OnTriggerExit(PhysicsComponent* other)
{
    cout << "InteractionZone : " << other->GetName() << endl;
    if (other->GetOwner()->GetName() == "SearchOB_Item")
    {
        player->SetCurSearchObject(nullptr);
        // TODO :: UI
    }
}
