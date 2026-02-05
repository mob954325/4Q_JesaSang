#include "JesaSangManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"
#include "EngineSystem/SceneSystem.h"
#include "../Item/Item.h"
#include "../Manager/GameManager.h"

REGISTER_COMPONENT(JesaSangManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<JesaSangManager>("JesaSangManager")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void JesaSangManager::OnInitialize()
{
    // singleton
    s_instance = this;
}

void JesaSangManager::OnStart()
{
    // gameobject find
    const auto& sceneSystem = SceneSystem::Instance().GetCurrentScene();
    apple = sceneSystem->GetGameObjectByName("Apple");
    pear = sceneSystem->GetGameObjectByName("Pear");
    batter = sceneSystem->GetGameObjectByName("Batter");
    tofu = sceneSystem->GetGameObjectByName("Tofu");
    sanjeok = sceneSystem->GetGameObjectByName("Sanjeok");
    dong = sceneSystem->GetGameObjectByName("Donggeurangttaeng");

    if (!apple || !pear || !batter || !sanjeok || !dong)
        std::cout << "[JesaSangManager] Missing GameObject!" << endl;
}

void JesaSangManager::OnDestory()
{
   
}

nlohmann::json JesaSangManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void JesaSangManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

bool JesaSangManager::HasAllFood()
{
    return hasApple && hasPear && hasBatter && hasTofu && hasSanjeok && hasDong;
}

/*
    [Food Item ID]
    Apple
    Pear
    Batter
    Tofu
    Sanjeok
    Donggeurangttaeng
*/
void JesaSangManager::ReceiveFood(std::unique_ptr<IItem> food)
{
    string foodID = food->itemId;
    if (foodID.empty())
    {
        std::cout << "[JesaSangManager] Food ID NULL!" << endl;
        return;
    }

    // 음식 올리기
    if (foodID == "Apple" && apple)
    {
        hasApple = true;
        apple->SetActive(true);
    }
    else if (foodID == "Pear" && pear)
    {
        hasPear = true;
        pear->SetActive(true);
    }
    else if (foodID == "Batter" && batter)
    {
        hasBatter = true;
        batter->SetActive(true);
    }
    else if (foodID == "Tofu" && tofu)
    {
        hasTofu = true;
        tofu->SetActive(true);
    }
    else if (foodID == "Sanjeok" && sanjeok)
    {
        hasSanjeok = true;
        sanjeok->SetActive(true);
    }
    else if (foodID == "Donggeurangttaeng" && dong)
    {
        hasDong = true;
        dong->SetActive(true);
    }

    // 전달받은 Food Item은 할일 끝났으니 쏘멸
    std::cout << "[JesaSangManager] Put Food : " << food->itemId << endl;
    food.reset();

    // 제사상에 음식이 다 올라오면 성공
    if(HasAllFood())
        GameManager::Instance()->GameSuccess();
}