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

bool Inventory::IsFull()
{
    return ingredient != nullptr;
}

void Inventory::AddItem(std::unique_ptr<IItem> item)
{
    if (item->itemType == ItemType::Ingredient)
    {
        ingredient = std::move(item);
        std::cout << "[Inventory] Add Ingredient : " << ingredient->itemId << std::endl;
    }
    else if (item->itemType == ItemType::Piece)
    {
        pieces.push_back(std::move(item));
        std::cout << "[Inventory] Add Piece" << std::endl;
    }
}

void Inventory::DropItem()
{
    ingredient.reset();
    // TODO :: 재단에 올라감
}

std::unique_ptr<IItem> Inventory::TakeCurItem()
{
    return std::move(ingredient);
}