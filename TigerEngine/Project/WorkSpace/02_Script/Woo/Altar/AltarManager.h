#pragma once
#include <memory>
#include <deque>
#include <string>
#include <unordered_map>
#include "Components/ScriptComponent.h"

class GameObject;
class IItem;

/*
    [ AltarManager Script Component ] <Singleton>

    플레이어가 떨어뜨린 아이템(음식재료/완성된 음식)이 올라오게 됩니다.
    플레이어는 제단에 있는 아이템을 다시 회수해갈 수 있습니다.
    
    제단에는 한번에 1개의 아이템만 올라와있을 수 있습니다. (퀘스트 시스템 제어)
*/

class AltarManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // singleton
    inline static AltarManager* s_instance = nullptr;

    // item
    std::unique_ptr<IItem> curItem;     // 현재 제단에 올라와있는 아이템 (1개)

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
    void OnInitialize() override;
    void OnStart() override;
    void OnDestory() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    // visual on/off
    void VisualItem(std::string itemID, bool isActive);

public:
    // 외부 call fucns..
    static AltarManager* Instance() { return s_instance; }

    bool HasItem() const { return (curItem != nullptr); }
    void ReceiveItem(std::unique_ptr<IItem> item);  // 제단에 아이템 올리기
    std::unique_ptr<IItem> GetItem();               // 제단 아아템 회수하기
};

