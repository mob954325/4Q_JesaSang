#pragma once
#include <memory>
#include <deque>
#include <string>
#include <unordered_map>
#include "Components/ScriptComponent.h"

class GameObject;
class Image;
class IItem;

/*
    [ AltarManager Script Component ] <Singleton>

    플레이어가 떨어뜨린 아이템(음식재료/완성된 음식)이 올라오게 됩니다.
    플레이어는 제단에 있는 아이템을 다시 회수해갈 수 있습니다.

    제단은 초기 비활성화 되어있으며, 제단에 아이템이 처음 올라간 순간 활성화 됩니다.
    

    0209 삭제 | 제단에는 한번에 1개의 아이템만 올라와있을 수 있습니다. (퀘스트 시스템 제어)
    0209 갱신 | 제단에는 여러개의 아이템이 올라올 수 있습니다.
                플레이어는 여전히 하나의 아이템만 회수 가능하며 FIFO 입니다.
                하지만 queue에 완성된 음식과 음식 재료가 함께 올라와있다면 
                완성된 음식을 우선적으로 회수합니다.
*/

class AltarManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // singleton
    inline static AltarManager* s_instance = nullptr;

    // item queue (FIFO)
    std::deque<std::unique_ptr<IItem>> foodQueue;       // 회수 우선순위 높음
    std::deque<std::unique_ptr<IItem>> ingreQueue;

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

    // child UI
    Image* image_sensorOn = nullptr;
    Image* image_interactionOn = nullptr;
    Image* image_interactionGauge = nullptr;

private:
    // active flag
    GameObject* altar = nullptr;     // 활성화/비활성화할 제단 덩어리 부모 게임오브젝트
    bool isFirstReceiveItem = false;
    void FirstReceiveDirect();       // 최초 제단 활성화


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

    // Item
    bool HasItem() const { return !(foodQueue.empty() && ingreQueue.empty()); }
    size_t GetItemCount() const { return foodQueue.size() + ingreQueue.size(); }

    void ReceiveItem(std::unique_ptr<IItem> item);  // 제단에 아이템 올리기
    std::unique_ptr<IItem> GetItem();               // 제단 아아템 회수하기 (FIFO)

    // UI
    void UISensorOnOff(bool flag);              // 플레이어 감지영역 UI
    void UIInteractionOnOff(bool flag);         // 플레이어 상호작용 영역 UI
    void UIGaugeUpate(float progress);
};

