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
    if (other->GetOwner()->GetName() == "SearchOB_Item")
    {
        player->SetCurSearchObject(other->GetOwner()->GetComponent<SearchObject>());
        // TODO :: UI
    }
}

void InteractionZone::OnTriggerStay(PhysicsComponent* other)
{
    // TODO :: Stay는 테스트용임. 나중에 삭제
    // 트리거 이벤트 호출 되는건지 확인 필요함
    cout << "InteractionSensor : " << other->GetName() << endl;
}

void InteractionZone::OnTriggerExit(PhysicsComponent* other)
{
    if (other->GetOwner()->GetName() == "SearchOB_Item")
    {
        player->SetCurSearchObject(nullptr);
        // TODO :: UI
    }
}
