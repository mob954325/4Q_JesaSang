#pragma once
#include "Components/ScriptComponent.h"

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

    // data
    float hideDurationTime = 10.0f;     // 최대 은신 시간
    float hideCoolTime = 1.0f;          // 재은신 쿨타임

    // controlls
    bool isAILooking = false;           // AI가 이 오브젝트를 보고 있는가
    bool isHiding = false;              // 현재 은신 중인가
    bool isCoolTime = false;            // 쿨타임이 찼는가

    float hideDurationTimer;
    float hideCoolTimer;

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
    void SetAILook(bool isLook);
};

