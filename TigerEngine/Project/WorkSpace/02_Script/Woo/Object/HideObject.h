#pragma once
#include "Components/ScriptComponent.h"

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


    // data
    float hideDurationTime = 10.0f;     // 숨어있을 수 있는 시간
    float hideCoolTime = 1.0f;          // 한번 숨고 다시 숨을 수 있는 쿨타임

    // controlls
    bool  isPossibleHide = false;        // 어른 유령 시야 x + CoolTime
    float hideDurationTimer = 10.0f;
    float hideCoolTimer = 1.0f;

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
    void SetAILook(bool isLook);
};

