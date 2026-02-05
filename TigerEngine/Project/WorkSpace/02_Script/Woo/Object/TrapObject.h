#pragma once
#include "Components/ScriptComponent.h"

/*
    [ TrapObject Script Component ]

    함정 오브젝트입니다.
    트리거 범위 안에 플레이어가 들어왔을 경우 파장을 발생시키고,
    그 범위 내의 AI를 호출합니다.

    재발동 쿨타임 : 5초

    - 함정 오브젝트 종류
     1) 깨진 바닥 (Decal)
     2) 낡은 책상 (Model)
*/

class TrapObject : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // component process
    void OnStart() override;
    void OnUpdate(float delta) override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);


public:
    // 외부 call funcs..

};

