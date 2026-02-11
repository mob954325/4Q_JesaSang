#pragma once
#include "Components/ScriptComponent.h"
#include <vector>

class Effect;
class PlayerController;
class AdultGhostController;
enum class AdultGhostState;


/*
    [ PlayerThreatMonitor Script Component ]

    씬의 AI 상태를 추적하여
    한마리라도 플레이어를 추적/탐색 하고있다면
    플레이어에게 검은 연기 이펙트를 발생시킵니다.

    - 모든 AI가 추적/탐색 모드가 아니라면 이펙트 Stop
    - 플레이어가 Hit상태에 들어갔을떄 이펙트 Stop
*/

class PlayerThreatMonitor : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)

private:
    // AI
    std::vector<AdultGhostController*> enemyControllers{};
    
    // Player
    PlayerController* pc;

    // Effect
    Effect* smokeEffect = nullptr;

    // controll
    bool isSmokeActive = false;

public:
    // component process
    void OnStart() override;
    void OnUpdate(float delta) override;

    // json
    nlohmann::json Serialize();
    void Deserialize(nlohmann::json data);

private:
    // funcs..
    void StartSmokeEffect();
    void StopSmokeEffect();

    bool ShouldEnableThreatEffect() const;
    bool IsAnyEnemyThreatening() const;
    bool IsEnemyThreateningState(AdultGhostState state) const;

    bool IsPlayerInHitState() const;

public:
    // 외부 call funcs..
};

