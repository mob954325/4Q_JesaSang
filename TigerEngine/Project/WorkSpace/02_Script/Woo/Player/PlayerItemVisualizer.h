#pragma once
#include "Components/ScriptComponent.h"
#include "../Item/Item.h"

class FBXRenderer;

/*
    [ PlayerItemVisualizer Script Component ]

    플레이어의 inventory에 있는 Ingrediant와 Food의 비주얼 연출을 관리하는 컴포넌트
    플레이어 머리 위에 뜰 Item(ingredient/food)의 On, Off, 연출을 관리합니다.
*/

class PlayerItemVisualizer : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // cur visual
    std::string curItemID = "";

    // visual gameobjects
    GameObject* ingre_apple = nullptr;
    GameObject* ingre_pear = nullptr;
    GameObject* ingre_batter = nullptr;
    GameObject* ingre_tofu = nullptr;
    GameObject* ingre_sanjeok = nullptr;
    GameObject* ingre_dong = nullptr;

    GameObject* food_apple = nullptr;
    GameObject* food_pear = nullptr;
    GameObject* food_batter = nullptr;
    GameObject* food_tofu = nullptr;
    GameObject* food_sanjeok = nullptr;
    GameObject* food_dong = nullptr;

public:
    // component process
    void OnStart() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

public:
    // 외부 call funcs..
    void VisualOnItem(std::string itemID);      // GameObect Active
    void VisualOffItem();                       // GameObect Active

    void VisualRenderOn();          // Render Active (Hide용)
    void VisualRenderOff();         // Render Active (Hide용)

    void VisualItemIDNullSet();     // hit으로 떨어뜨리고 hide->exit시 마지막 id가 남아있어서 추가
};

