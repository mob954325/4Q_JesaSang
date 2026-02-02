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
    Vector3 upatePos = tr->GetParent()->GetOwner()->GetTransform()->GetWorldPosition() + Vector3(0,20,0);
    tr->SetPosition(upatePos);
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
    if (other->GetOwner()->GetName() == "SearchObject")
    {
        player->SetCurSearchObject(other->GetOwner()->GetComponent<SearchObject>());
        cout << "[InteractionZone] SearchObject In Interaction Zone" << endl;
        // TODO :: UI
    }
}

void InteractionZone::OnTriggerExit(PhysicsComponent* other)
{
    if (other->GetOwner()->GetName() == "SearchObject")
    {
        player->SetCurSearchObject(nullptr);
        cout << "[InteractionZone] SearchObject Out Interaction Zone" << endl;
        // TODO :: UI
    }
}
