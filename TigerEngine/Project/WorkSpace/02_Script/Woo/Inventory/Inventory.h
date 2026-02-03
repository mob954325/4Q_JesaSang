#pragma once
#include "Components/ScriptComponent.h"
#include "../Item/Item.h"

/*
    [ Player Inventory Script Component ]

    플레이어가 소지중인 Item을 관리하는 인벤토리입니다.
    Slot 1칸의 Item(음식재료/완성된 음식)을 관리하는것이 메인 목적입니다.
    지도조각은 필요 없을것같지만 유지보수측면에서 걍 넣어둠

    - Item -
     1) Ingredient : 음식재료
     2) Piece : 지도조각
     3) Food : 완성된 음식
*/

class Inventory : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // components
    

    // inventory items
    std::unique_ptr<IItem> item;                     // ingredient, food
    std::vector<std::unique_ptr<IItem>> pieces;      // pieces

public:
    // component process
    void OnStart() override;
    void OnDestory() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    // funcs..
    

public:
    // 외부 call fucns..
    bool HasItem();
    void AddItem(std::unique_ptr<IItem> item);
    void DropItem();

    std::string GetCurItemID();
    std::unique_ptr<IItem> TakeCurItem();
};

