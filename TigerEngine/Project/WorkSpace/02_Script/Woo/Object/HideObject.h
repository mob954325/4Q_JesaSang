#pragma once
#include "Components/ScriptComponent.h"

class GameObject;
class Image;
class PlayerController;

/*
    [ HideObject Script Component ]

    플레이어가 Hide(은신) 할 수 있는 오브젝트입니다.
    AI의 시야범위 안에 없고, 쿨타임이 찼을때 은신이 가능합니다.

    은신 지속 최대 시간 : 10초
    재 은신 쿨타임 : 1초

    - 어떤 모델있는지 리스트업
*/

class HideObject : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // components
    PlayerController* player = nullptr;

    // child UI
    Image* image_sensorOn = nullptr;
    Image* image_interactionOn = nullptr;
    Image* image_interactionGauge = nullptr;
   
    // data
    float hideDurationTime = 10.0f;     // 최대 은신 시간
    float reHideCoolTime = 1.0f;        // 재은신 쿨타임

    // controlls
    bool isAILooking = false;           // AI가 이 오브젝트를 보고 있는가
    bool isHiding = false;              // 현재 은신 중인가
    bool reHideReady = false;           // 재은신 쿨타임이 찼는가

    float hideDurationTimer = 0.0f;
    float reHideCoolTimer = 0.0f;


    // 자개장은 추가 연출 필요
    bool isJagaejang = false;
    GameObject* openModel = nullptr;
    GameObject* closeModel = nullptr;
    void UpdateJagaejangModel();


public:
    // component process
    void OnStart() override;
    void OnUpdate(float delta) override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);


public : 
    // 외부 call funcs..
    bool IsPossibleHide();
    void StartHide(PlayerController* p);
    void StopHide();

    // UI
    void UISensorOnOff(bool flag);              // 플레이어 감지영역 UI
    void UIInteractionOnOff(bool flag);         // 플레이어 상호작용 영역 UI
    void UIGaugeUpate(float progress);          // 플레이어 인터랙션 UI


    // AI 시야 연결 - 선민 | 02.07 
    // 해당 HideObject를 바라보고 있는 AI들을 Set으로 관리해서 
    // 한 마리라도 보고 있으면 isAILooking = true
private:
    std::unordered_set<void*> lookingAIs;
    void UpdateAILook()
    {
        isAILooking = !lookingAIs.empty();
        std::cout << "[HideObject] isAILooking = " << isAILooking << std::endl;
    }

public:
    void RegisterAILook(void* ai)
    {
        lookingAIs.insert(ai);
        UpdateAILook();
    }

    void UnregisterAILook(void* ai)
    {
        lookingAIs.erase(ai);
        UpdateAILook();
    }

    bool IsAILooking() const { return isAILooking; }
};