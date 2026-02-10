#include "AltarManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "EngineSystem/SceneSystem.h"
#include "EngineSystem/CameraSystem.h"
#include "Components/Camera.h"
#include "Object/GameObject.h"
#include "Components/UI/Image.h"
#include "Manager/WorldManager.h"

#include "../Item/Item.h"

REGISTER_COMPONENT(AltarManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<AltarManager>("AltarManager")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void AltarManager::FirstReceiveDirect(std::string itemId)
{
    // 제단 활성화 연출
    /*
    - 페이드아웃 → 페이드인 (이때 카메라가 제단을 정면에서 바라보는 view)
    - 카메라 줌인 → 제단에 푸른 불꽃 이펙트를 재생하며 음식 비주얼 on
    - 페이드아웃 → 페이드인(이때 원시점 복귀)
    */

    // 1. FadeOut

    // 2. 카메라 전환 (Direct Camera)
    CameraSystem::Instance().SetCurrCameraByName(camName);
    
    // 3. 최초 제단 활성화
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

    // 4. FadeIn + Cam position udpate (z-)

    // 5. Effect Play + Visual On
    VisualItem(itemId, true);
    

    // 6. FadeOut

    // 7. 카메라 전환 (Main Camera)
    CameraSystem::Instance().SetCurrCameraByName("MainCamera");

    // 8. FadeIn
}

void AltarManager::OnInitialize()
{
    // singleton
    s_instance = this;
}

void AltarManager::OnStart()
{
    const auto& sceneSystem = SceneSystem::Instance().GetCurrentScene();

    // direct cam find
    altarDirectCam = sceneSystem->GetGameObjectByName("AltarDirectCamera")->GetComponent<Transform>();
    altarDirectCam->GetOwner()->SetName(camName);

    // gameobject find
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

    image_sensorOn = sceneSystem->GetGameObjectByName("Image_SensorOn_Altar")->GetComponent<Image>();
    image_interactionOn = sceneSystem->GetGameObjectByName("Image_InteractionOn_Altar")->GetComponent<Image>();
    image_interactionGauge = sceneSystem->GetGameObjectByName("Image_InteractionGauge_Altar")->GetComponent<Image>();

    if (!image_sensorOn || !image_interactionOn || !image_interactionGauge)
    {
        cout << "[AltarManager] Missing ui!" << endl;
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

    image_sensorOn->SetActive(false);
    image_interactionOn->SetActive(false);
    image_interactionGauge->SetActive(false);
}

void AltarManager::OnDestory()
{
    if (s_instance == this) s_instance = nullptr;

    itemQueue.clear();
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
        FirstReceiveDirect(item->itemId);
        itemQueue.push_back(std::move(item));
        return;     // 비주얼은 연출쪽에서 알맞은 타이밍에 on
    }

    // 비주얼 on
    VisualItem(item->itemId, true);

    // 아이템이 제단에 올라감
    itemQueue.push_back(std::move(item));
}

std::unique_ptr<IItem> AltarManager::GetItem()
{
    // FIFO 회수
    if (!itemQueue.empty())
    {
        std::unique_ptr<IItem> out = std::move(itemQueue.front());
        itemQueue.pop_front();

        VisualItem(out->itemId, false);

        // UI clear
        UISensorOnOff(false);
        UIInteractionOnOff(false);
        image_interactionGauge->SetFillAmount(0.0);

        return out;
    }

    cout << "[AltarManager] no item !" << endl;
    return nullptr;
}

void AltarManager::UISensorOnOff(bool flag)
{
    if (!image_sensorOn) return;
    if (flag && !isFirstReceiveItem) return;
    image_sensorOn->SetActive(flag);
}

void AltarManager::UIInteractionOnOff(bool flag)
{
    if (!image_interactionOn) return;
    if (flag && !isFirstReceiveItem) return;
    image_interactionOn->SetActive(flag);
    image_interactionGauge->SetActive(flag);
}

void AltarManager::UIGaugeUpate(float progress)
{
    if (!image_interactionGauge) return;
    if (!isFirstReceiveItem) return;
    image_interactionGauge->SetFillAmount(progress);
}