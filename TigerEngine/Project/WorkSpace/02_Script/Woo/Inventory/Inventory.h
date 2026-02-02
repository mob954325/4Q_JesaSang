#pragma once
#include "Components/ScriptComponent.h"
#include "../Item/Item.h"

/*
    [ Player Inventory Script Component ]

    플레이어가 소지중인 Item을 관리하는 인벤토리입니다.
    Slot 1칸의 Ingredient을 관리하는것이 메인 목적입니다.
    그래서 함수명의 Item은 그냥 ingredient라고 생각하묜 됨
    지도조각은 필요 없을것같지만 유지보수측면에서 걍 넣어둠

    - Item -
     1) Ingredient : 음식재료
     2) Piece : 지도조각
*/

class Inventory : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // components
    

    // inventory items
    std::unique_ptr<IItem> ingredient;
    std::vector<std::unique_ptr<IItem>> pieces;

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
    bool IsFull();
    void AddItem(std::unique_ptr<IItem> item);
    void DropItem();
    std::unique_ptr<IItem> TakeCurItem();
};

