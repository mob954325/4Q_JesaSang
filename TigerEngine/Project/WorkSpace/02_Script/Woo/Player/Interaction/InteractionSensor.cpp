#include "InteractionSensor.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"


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
    if (other->GetOwner()->GetName() == "SearchOB_Item")
    {
        cout << "[Player] Object In Interaction Sensor" << endl;
    }
}

void InteractionSensor::OnTriggerStay(PhysicsComponent* other)
{
    // 트리거 이벤트 호출 되는건지 확인 필요함
    cout << "InteractionSensor : " << other->GetName() << endl;
}

void InteractionSensor::OnTriggerExit(PhysicsComponent* other)
{
    if (other->GetOwner()->GetName() == "SearchOB_Item")
    {
        cout << "[Player] Object Out Interaction Sensor" << endl;
    }
}
