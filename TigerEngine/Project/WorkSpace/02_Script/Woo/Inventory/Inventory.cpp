#include "Inventory.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"

REGISTER_COMPONENT(Inventory)

RTTR_REGISTRATION
{
    rttr::registration::class_<Inventory>("Inventory")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void Inventory::OnStart()
{
   
}

void Inventory::OnDestory()
{
}

nlohmann::json Inventory::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void Inventory::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

bool Inventory::HasItem()
{
    return item != nullptr;
}

void Inventory::AddItem(std::unique_ptr<IItem> item)
{
    if (item->itemType == ItemType::Ingredient || item->itemType == ItemType::Food)
    {
        this->item = std::move(item);
        std::cout << "[Inventory] Add Item : " << this->item->itemId << std::endl;
    }
    else if (item->itemType == ItemType::Piece)
    {
        pieces.push_back(std::move(item));
        std::cout << "[Inventory] Add Piece" << std::endl;
    }
}

std::string Inventory::GetCurItemID()
{
    if (item) return item->itemId;
    else return "";
}

ItemType Inventory::GetCurItemType()
{
    if (item) return item->itemType;
    else return ItemType::None;
}

std::unique_ptr<IItem> Inventory::TakeCurItem()
{
    return std::move(item);
}