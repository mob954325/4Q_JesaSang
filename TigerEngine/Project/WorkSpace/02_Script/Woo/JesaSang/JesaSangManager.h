#pragma once
#include "Components/ScriptComponent.h"

class GameObject;
class Image;
class IItem;

/*
    [ JesaSangManager Script Component ] <Singleton>

    플레이어 인터랙션으로 음식을 전달받고,
    제사상 테이블의 음식 모델 on/off를 관리합니다.
    게임 성공 데이터와 연결될 가능성이 있습니다. (GameManager)
*/

class JesaSangManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // singleton
    inline static JesaSangManager* s_instance = nullptr;

    // has item (현재 제사상에 올라온 음식)
    bool hasApple = false;
    bool hasPear = false;
    bool hasBatter = false;
    bool hasTofu = false;
    bool hasSanjeok = false;
    bool hasDong = false;

    // visual gameobjects
    GameObject* apple = nullptr;
    GameObject* pear = nullptr;
    GameObject* batter = nullptr;
    GameObject* tofu = nullptr;
    GameObject* sanjeok = nullptr;
    GameObject* dong = nullptr;

    // child UI
    Image* image_sensorOn = nullptr;
    Image* image_interactionOn = nullptr;
    Image* image_interactionGauge = nullptr;


public:
    // component process
    void OnInitialize() override;
    void OnStart() override;
    void OnDestory() override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    // funcs
    bool HasAllFood();

public:
    // 외부 call fucns..
    static JesaSangManager* Instance() { return s_instance; }

    void ReceiveFood(std::unique_ptr<IItem> food);

    // UI
    void UISensorOnOff(bool flag);              // 플레이어 감지영역 UI
    void UIInteractionOnOff(bool flag);         // 플레이어 상호작용 영역 UI
    void UIGaugeUpate(float progress);
};

