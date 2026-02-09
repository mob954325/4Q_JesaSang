#include "AltarManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"
#include "EngineSystem/SceneSystem.h"
#include "../Item/Item.h"

REGISTER_COMPONENT(AltarManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<AltarManager>("AltarManager")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void AltarManager::FirstReceiveDirect()
{
    // 최초 제단 활성화
    altar->SetActive(true);

    ingre_apple->SetActive(false);
    ingre_pear->SetActive(false);
    ingre_batter->SetActive(false);
    ingre_tofu->SetActive(false);
    ingre_sanjeok->SetActive(false);
    ingre_dong->SetActive(false);

    food_apple->SetActive(false);
    food_pear->SetActive(false);
    food_batter->SetActive(false);
    food_tofu->SetActive(false);
    food_sanjeok->SetActive(false);
    food_dong->SetActive(false);

    // TODO :: 제단 활성화 연출 여기에 연결
}

void AltarManager::OnInitialize()
{
    // singleton
    s_instance = this;
}

void AltarManager::OnStart()
{
    // gameobject find
    const auto& sceneSystem = SceneSystem::Instance().GetCurrentScene();

    altar = sceneSystem->GetGameObjectByName("Altar");

    ingre_apple = sceneSystem->GetGameObjectByName("Alta_Ingre_Apple");
    ingre_pear = sceneSystem->GetGameObjectByName("Alta_Ingre_Pear");
    ingre_batter = sceneSystem->GetGameObjectByName("Alta_Ingre_Batter");
    ingre_tofu = sceneSystem->GetGameObjectByName("Alta_Ingre_Tofu");
    ingre_sanjeok = sceneSystem->GetGameObjectByName("Alta_Ingre_Sanjeok");
    ingre_dong = sceneSystem->GetGameObjectByName("Alta_Ingre_Donggeurangttaeng");

    food_apple = sceneSystem->GetGameObjectByName("Alta_Apple");
    food_pear = sceneSystem->GetGameObjectByName("Alta_Pear");
    food_batter = sceneSystem->GetGameObjectByName("Alta_Batter");
    food_tofu = sceneSystem->GetGameObjectByName("Alta_Tofu");
    food_sanjeok = sceneSystem->GetGameObjectByName("Alta_Sanjeok");
    food_dong = sceneSystem->GetGameObjectByName("Alta_Donggeurangttaeng");

    if (!ingre_apple || !ingre_pear || !ingre_batter || !ingre_tofu || !ingre_sanjeok || !ingre_dong ||
        !food_apple || !food_pear || !food_batter || !food_tofu || !food_sanjeok || !food_dong)
    {
        cout << "[AltarManager] Missing GameObject!" << endl;
        return;
    }

    altar->SetActive(false);

    ingre_apple->SetActive(false);
    ingre_pear->SetActive(false);
    ingre_batter->SetActive(false);
    ingre_tofu->SetActive(false);
    ingre_sanjeok->SetActive(false);
    ingre_dong->SetActive(false);

    food_apple->SetActive(false);
    food_pear->SetActive(false);
    food_batter->SetActive(false);
    food_tofu->SetActive(false);
    food_sanjeok->SetActive(false);
    food_dong->SetActive(false);
}

void AltarManager::OnDestory()
{
    if (s_instance == this) s_instance = nullptr;

    foodQueue.clear();
    ingreQueue.clear();
}

nlohmann::json AltarManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void AltarManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

/*
    [Item ID]

    Ingredient_Apple
    Ingredient_Pear
    Ingredient_Batter
    Ingredient_Tofu
    Ingredient_Sanjeok
    Ingredient_Donggeurangttaeng

    Apple
    Pear
    Batter
    Tofu
    Sanjeok
    Donggeurangttaeng
*/

void AltarManager::VisualItem(std::string itemID, bool isActive)
{
    if (itemID.empty()) return;

    // Ingredient
    if (itemID == "Ingredient_Apple" && ingre_apple) ingre_apple->SetActive(isActive);
    else if (itemID == "Ingredient_Pear" && ingre_pear) ingre_pear->SetActive(isActive);
    else if (itemID == "Ingredient_Batter" && ingre_batter) ingre_batter->SetActive(isActive);
    else if (itemID == "Ingredient_Tofu" && ingre_tofu) ingre_tofu->SetActive(isActive);
    else if (itemID == "Ingredient_Sanjeok" && ingre_sanjeok) ingre_sanjeok->SetActive(isActive);
    else if (itemID == "Ingredient_Donggeurangttaeng" && ingre_dong) ingre_dong->SetActive(isActive);

    // Food
    else if (itemID == "Apple" && food_apple) food_apple->SetActive(isActive);
    else if (itemID == "Pear" && food_pear) food_pear->SetActive(isActive);
    else if (itemID == "Batter" && food_batter) food_batter->SetActive(isActive);
    else if (itemID == "Tofu" && food_tofu) food_tofu->SetActive(isActive);
    else if (itemID == "Sanjeok" && food_sanjeok) food_sanjeok->SetActive(isActive);
    else if (itemID == "Donggeurangttaeng" && food_dong) food_dong->SetActive(isActive);
    else
        cout << "[AltarManager] Unknown itemID: " << itemID << endl;
}

void AltarManager::ReceiveItem(std::unique_ptr<IItem> item)
{
    if (!item) {
        cout << "[AltarManager] item null !" << endl;
        return;
    }

    // 최초 제단 활성화
    if (!isFirstReceiveItem)
    {
        isFirstReceiveItem = true;
        FirstReceiveDirect();
    }

    // 비주얼 on
    VisualItem(item->itemId, true);

    // 아이템이 제단에 올라감
    if (item->itemType == ItemType::Food)
        foodQueue.push_back(std::move(item));
    else
        ingreQueue.push_back(std::move(item));
}

std::unique_ptr<IItem> AltarManager::GetItem()
{
    // 완성된 음식 우선 회수 (FIFO)
    if (!foodQueue.empty() && foodQueue.front())
    {
        std::unique_ptr<IItem> out = std::move(foodQueue.front());
        foodQueue.pop_front();

        VisualItem(out->itemId, false);

        return out;
    }

    // 음식 재료 회수 (FIFO)
    if (!ingreQueue.empty() && ingreQueue.front())
    {
        std::unique_ptr<IItem> out = std::move(ingreQueue.front());
        ingreQueue.pop_front();

        VisualItem(out->itemId, false);

        return out;
    }

    cout << "[AltarManager] no item !" << endl;
    return nullptr;
}
