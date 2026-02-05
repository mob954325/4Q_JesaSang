#include "SearchObject.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

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

std::unique_ptr<IItem> SearchObject::Interaction()
{
    if (isSearched) return nullptr;

    // 수색한 오브젝트 표시
    fbxRenderer->SetDiffuse(interactionDiffuse);
    isSearched = true;

    // 플레이어 아이템 습득
    if (!hasItem)
    {
        SetActive(false);
        return nullptr;
    }

    SetActive(false);
    return std::move(item);
}
