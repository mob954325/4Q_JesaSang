#include "PlayerItemVisualizer.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"
#include "EngineSystem/SceneSystem.h"

REGISTER_COMPONENT(PlayerItemVisualizer)

RTTR_REGISTRATION
{
    rttr::registration::class_<PlayerItemVisualizer>("PlayerItemVisualizer")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void PlayerItemVisualizer::OnStart()
{
    // gameobject find
    const auto& sceneSystem = SceneSystem::Instance().GetCurrentScene();
    ingre_apple = sceneSystem->GetGameObjectByName("Visual_Ingre_Apple");
    ingre_pear = sceneSystem->GetGameObjectByName("Visual_Ingre_Pear");
    ingre_batter = sceneSystem->GetGameObjectByName("Visual_Ingre_Batter");
    ingre_tofu = sceneSystem->GetGameObjectByName("Visual_Ingre_Tofu");
    ingre_sanjeok = sceneSystem->GetGameObjectByName("Visual_Ingre_Sanjeok");
    ingre_dong = sceneSystem->GetGameObjectByName("Visual_Ingre_Donggeurangttaeng");

    food_apple = sceneSystem->GetGameObjectByName("Visual_Apple");
    food_pear = sceneSystem->GetGameObjectByName("Visual_Pear");
    food_batter = sceneSystem->GetGameObjectByName("Visual_Batter");
    food_tofu = sceneSystem->GetGameObjectByName("Visual_Tofu");
    food_sanjeok = sceneSystem->GetGameObjectByName("Visual_Sanjeok");
    food_dong = sceneSystem->GetGameObjectByName("Visual_Donggeurangttaeng");

    if (!ingre_apple || !ingre_pear || !ingre_batter || !ingre_sanjeok || !ingre_dong ||
        !food_apple || !food_pear || !food_batter || !food_sanjeok || !food_dong)
        std::cout << "[JesaSangManager] Missing GameObject!" << endl;
}

nlohmann::json PlayerItemVisualizer::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void PlayerItemVisualizer::Deserialize(nlohmann::json data)
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

void PlayerItemVisualizer::VisualOnItem(std::string itemID)
{
    if (itemID.empty()) return;

    // all off
    VisualOffItem();

    // Ingredient
    if (itemID == "Ingredient_Apple" && ingre_apple)
        ingre_apple->SetActive(true);
    else if (itemID == "Ingredient_Pear" && ingre_pear)
        ingre_pear->SetActive(true);
    else if (itemID == "Ingredient_Batter" && ingre_batter)
        ingre_batter->SetActive(true);
    else if (itemID == "Ingredient_Tofu" && ingre_tofu)
        ingre_tofu->SetActive(true);
    else if (itemID == "Ingredient_Sanjeok" && ingre_sanjeok)
        ingre_sanjeok->SetActive(true);
    else if (itemID == "Ingredient_Donggeurangttaeng" && ingre_dong)
        ingre_dong->SetActive(true);

    // Food
    else if (itemID == "Apple" && food_apple)
        food_apple->SetActive(true);
    else if (itemID == "Pear" && food_pear)
        food_pear->SetActive(true);
    else if (itemID == "Batter" && food_batter)
        food_batter->SetActive(true);
    else if (itemID == "Tofu" && food_tofu)
        food_tofu->SetActive(true);
    else if (itemID == "Sanjeok" && food_sanjeok)
        food_sanjeok->SetActive(true);
    else if (itemID == "Donggeurangttaeng" && food_dong)
        food_dong->SetActive(true);
}

void PlayerItemVisualizer::VisualOffItem()
{
    // Ingredient
    if (ingre_apple)   ingre_apple->SetActive(false);
    if (ingre_pear)    ingre_pear->SetActive(false);
    if (ingre_batter)  ingre_batter->SetActive(false);
    if (ingre_tofu)    ingre_tofu->SetActive(false);
    if (ingre_sanjeok) ingre_sanjeok->SetActive(false);
    if (ingre_dong)    ingre_dong->SetActive(false);

    // Food
    if (food_apple)    food_apple->SetActive(false);
    if (food_pear)     food_pear->SetActive(false);
    if (food_batter)   food_batter->SetActive(false);
    if (food_tofu)     food_tofu->SetActive(false);
    if (food_sanjeok)  food_sanjeok->SetActive(false);
    if (food_dong)     food_dong->SetActive(false);
}
