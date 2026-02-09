#include "SearchObject.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

#include "Components/UI/Image.h"


REGISTER_COMPONENT(SearchObject)

RTTR_REGISTRATION
{
    rttr::registration::enumeration<ItemType>("ItemType")
        (
            rttr::value("Ingredient", ItemType::Ingredient),
            rttr::value("Piece", ItemType::Piece),
            rttr::value("None", ItemType::None)
        );

    rttr::registration::class_<SearchObject>("SearchObject")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr)
    
    .property("hasItem", &SearchObject::hasItem)
    .property("itemType", &SearchObject::itemType)
    .property("itemID", &SearchObject::itemID)
    .property("interactionDiffuse", &SearchObject::interactionDiffuse);
}

void SearchObject::OnStart()
{
    // get components
    fbxRenderer = GetOwner()->GetComponent<FBXRenderer>();
    if (!fbxRenderer)
    {
        cout << "[SearchObject] OnStart Missing 'fbxRenderer Component'" << endl;
        return;
    }

    // get child compoennts
    auto t1 = GetOwner()->GetChildByName("Image_SensorOn");
    auto t2 = GetOwner()->GetChildByName("Image_InteractionOn");
    auto t3 = GetOwner()->GetChildByName("Image_InteractionGauge");

    if (!t1 || !t2 || !t3)
    {
        cout << "[SearchObject] Missing child transforms!" << endl;
        return;
    }

    image_sensorOn = t1->GetOwner()->GetComponent<Image>();
    image_interactionOn = t2->GetOwner()->GetComponent<Image>();
    image_interactionGauge = t3->GetOwner()->GetComponent<Image>();

    // Item Create (음식재료/지도조각)
    if (hasItem)
    {
        if (itemType == ItemType::Ingredient)
            item = std::make_unique<Ingredient>(itemID);
        else if (itemType == ItemType::Piece)
            item = std::make_unique<Piece>(itemID);
    }
}

nlohmann::json SearchObject::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void SearchObject::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void SearchObject::UISensorOnOff(bool flag)
{
    if (!image_sensorOn) return;
    if (flag && isSearched) return;

    image_sensorOn->SetActive(flag);
}

void SearchObject::UIInteractionOnOff(bool flag)
{
    if (!image_interactionOn || !image_interactionGauge ) return;
    if (flag && isSearched) return;

    image_interactionOn->SetActive(flag);
    image_interactionGauge->SetActive(flag);
}

void SearchObject::UIGaugeUpate(float progress)
{
    if (!image_interactionGauge) return;
    if (isSearched) return;

    image_interactionGauge->SetFillAmount(progress);
}

std::unique_ptr<IItem> SearchObject::Interaction()
{
    if (isSearched) return nullptr;

    // UI 스스로 끄기 (모든 수색 오브젝트는 1회용임)
    UISensorOnOff(false);
    UIInteractionOnOff(false);

    // 수색한 오브젝트 표시
    fbxRenderer->SetDiffuse(interactionDiffuse);
    isSearched = true;

    // 플레이어 아이템 습득 실패(빈 수색 오브젝트)
    if (!hasItem)
    {
        SetActive(false);
        return nullptr;
    }

    // 플레이어 아이템 습득
    SetActive(false);
    return std::move(item);
}
