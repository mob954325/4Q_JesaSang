#include "InteractionSensor.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"


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
    tr->SetPosition(tr->GetParent()->GetOwner()->GetTransform()->GetWorldPosition());
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
    if (other->GetOwner()->GetName() == "SearchOB_Item")
    {
        // TODO :: UI
    }
}

void InteractionSensor::OnTriggerStay(PhysicsComponent* other)
{
    // TODO :: 이거 테스트용임. 추후 삭제
    // 트리거 이벤트 호출 되는건지 확인 필요함
    cout << "InteractionSensor : " << other->GetName() << endl;
}

void InteractionSensor::OnTriggerExit(PhysicsComponent* other)
{
    if (other->GetOwner()->GetName() == "SearchOB_Item")
    {
        // TODO :: UI
    }
}
