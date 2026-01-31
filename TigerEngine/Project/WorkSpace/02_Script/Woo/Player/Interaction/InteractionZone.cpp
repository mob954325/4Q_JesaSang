#include "InteractionZone.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"


REGISTER_COMPONENT(InteractionZone)

RTTR_REGISTRATION
{

    rttr::registration::class_<InteractionZone>("InteractionZone")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void InteractionZone::OnStart()
{
}

void InteractionZone::OnUpdate(float delta)
{
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
    if (other->GetOwner()->GetName() == "SearchOB_Item")
    {
        cout << "[Player] In Interaction Zone" << endl;
    }
}

void InteractionZone::OnTriggerStay(PhysicsComponent* other)
{
    cout << "InteractionSensor : " << other->GetName() << endl;
}

void InteractionZone::OnTriggerExit(PhysicsComponent* other)
{
    if (other->GetOwner()->GetName() == "SearchOB_Item")
    {
        cout << "[Player] Out Interaction Zone" << endl;
    }
}
