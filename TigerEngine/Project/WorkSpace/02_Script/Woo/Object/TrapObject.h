#pragma once
#include "Components/ScriptComponent.h"
class Decal;

/*
    [ TrapObject Script Component ]

    함정 오브젝트입니다.
    트리거 범위 안에 플레이어가 들어왔을 경우 플레이어 상태에 따른 파장을 발생시키고,
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
    // component
    Decal* ringEffect = nullptr;;

    // cur
    float waveRadius = 0.0f;

    // data
    float walkWaveRadius = 1.5;
    float runWaveRadius = 3.0;
    float coolTime = 5.0f;

    // rewave cool time
    bool isPossibleWave = true;
    bool isWaveing = false;
    float coolTimer = 0.0f;

public:
    // component process
    void OnStart() override;
    void OnUpdate(float delta) override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

    // trigger event
    void OnCCTTriggerEnter(CharacterControllerComponent* other) override;

private:
    void StartTriggerWave();        // 파장 웨이브
    void NotifyAIInRange();         // AI 찾아서 호출

public:
    // 외부 call funcs..

};

