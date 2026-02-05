#pragma once
#include "Components/ScriptComponent.h"
#include "Components/FBXRenderer.h"
#include "../Item/Item.h"

/*
    [ SearchObject Script Component ]

    Item(음식재료, 지도조각)이 있을수도(있고 없을수도) 있는 오브젝트
    플레이어가 집을 탐색하며 상호작용한다.

    - 장롱
*/

class SearchObject : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // components
    FBXRenderer* fbxRenderer = nullptr;
    
public:
    // data
    bool     hasItem;
    ItemType itemType = ItemType::None;
    string   itemID;
    Color    interactionDiffuse;

    // controlls
    std::unique_ptr<IItem> item = nullptr;
    bool  isSearched;

public:
    // component process
    void OnStart() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

public:
    // 외부 call funcs...
    std::unique_ptr<IItem> Interaction();       // 상호작용 Enter
};

