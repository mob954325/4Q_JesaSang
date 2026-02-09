#include "InteractionSensor.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"
#include "Components/FBXRenderer.h"

#include "../../Object/SearchObject.h"
#include "../../Object/HideObject.h"


REGISTER_COMPONENT(InteractionSensor)

RTTR_REGISTRATION
{

    rttr::registration::class_<InteractionSensor>("InteractionSensor")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void InteractionSensor::OnStart()
{
   
}

void InteractionSensor::OnUpdate(float delta)
{
    // transform->physics udpate
    auto ob = GetOwner();
    auto tr = ob->GetTransform();
    Vector3 upatePos = tr->GetParent()->GetOwner()->GetTransform()->GetWorldPosition() + Vector3(0, 30, 0);
    tr->SetPosition(upatePos);
    GetOwner()->GetComponent<PhysicsComponent>()->SyncToPhysics();
}

nlohmann::json InteractionSensor::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void InteractionSensor::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void InteractionSensor::OnTriggerEnter(PhysicsComponent* other)
{
    GameObject* object = other->GetOwner();

    // 수색오브젝트 감지 on
    if (object->GetName() == "SearchObject")
    {
        auto* so = object->GetComponent<SearchObject>();
        if (so)
            so->UISensorOnOff(true);
    }
}

void InteractionSensor::OnTriggerExit(PhysicsComponent* other)
{
    GameObject* object = other->GetOwner();

    // 수색오브젝트 감지 off
    if (object->GetName() == "SearchObject")
    {
        auto* so = object->GetComponent<SearchObject>();
        if (so)
            so->UISensorOnOff(false);
    }
}
