#include "InteractionZone.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "EngineSystem/PhysicsSystem.h"

#include "Object/GameObject.h"
#include "../../Object/SearchObject.h"
#include "../../Object/HideObject.h"


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
    Vector3 upatePos = tr->GetParent()->GetOwner()->GetTransform()->GetWorldPosition() + Vector3(0,70,0);
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
    // Search Object
    if (other->GetOwner()->GetName() == "SearchObject")
    {
        auto searchOB = other->GetOwner()->GetComponent<SearchObject>();
        if(searchOB && !searchOB->isSearched)
        {
            player->SetCurSearchObject(searchOB);
            searchOB->UIInteractionOnOff(true);
            cout << "[InteractionZone] SearchObject In Interaction Zone" << endl;
        }
    }

    // Hide Object
    if (other->GetOwner()->GetName() == "HideObject")
    {
        auto hideOB = other->GetOwner()->GetComponent<HideObject>();
        if (hideOB)
        {
            player->SetCurHideObject(hideOB);
            cout << "[InteractionZone] HideObject In Interaction Zone" << endl;
        }
    }

    // Kitchin (MiniGame)
    if (other->GetOwner()->GetName() == "CookingZone")
    {
        player->isPossibleCooking = true;
        cout << "[InteractionZone] CookingZone In Interaction Zone" << endl;
    }

    // JesaSang
    if (other->GetOwner()->GetName() == "JesaSang")
    {
        player->isPossiblePutFood = true;
        cout << "[InteractionZone] JesaSang In Interaction Zone" << endl;
    }

    // Altar
    if (other->GetOwner()->GetName() == "Altar")
    {
        player->isPossibleGetFood = true;
        cout << "[InteractionZone] Altar In Interaction Zone" << endl;
    }
}

void InteractionZone::OnTriggerExit(PhysicsComponent* other)
{
    // Search Object
    if (other->GetOwner()->GetName() == "SearchObject")
    {
        auto searchOB = other->GetOwner()->GetComponent<SearchObject>();
        if (searchOB && !searchOB->isSearched)
        {
            player->SetCurSearchObject(nullptr);
            searchOB->UIInteractionOnOff(false);
            cout << "[InteractionZone] SearchObject Out Interaction Zone" << endl;
        }
    }

    // Hide Object
    if (other->GetOwner()->GetName() == "HideObject")
    {
        auto hideOB = other->GetOwner()->GetComponent<HideObject>();
        if (hideOB)
        {
            player->SetCurHideObject(nullptr);
            cout << "[InteractionZone] HideObject Out Interaction Zone" << endl;
        }
    }

    // Kitchin (MiniGame)
    if (other->GetOwner()->GetName() == "CookingZone")
    {
        player->isPossibleCooking = false;
        cout << "[InteractionZone] CookingZone Out Interaction Zone" << endl;
    }

    // JesaSang
    if (other->GetOwner()->GetName() == "JesaSang")
    {
        player->isPossiblePutFood = false;
        cout << "[InteractionZone] JesaSang Out Interaction Zone" << endl;
    }

    // Altar
    if (other->GetOwner()->GetName() == "Altar")
    {
        player->isPossibleGetFood = false;
        cout << "[InteractionZone] Altar Out Interaction Zone" << endl;
    }
}
