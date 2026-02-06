#pragma once
#include "IAdultGhostState.h"

class AdultGhost_Chase : public IAdultGhostState
{
private:
    float chaseTimer = 0.0f;        // 추격 시작
    float forceChaseTime = 3.0f;

    float repathTimer = 0.f;        // 경로 재계산 타이머
    float repathInterval = 0.3f;    // 몇 초마다 재계산할지

    AgentComponent* agent = nullptr;
    GameObject* aiTarget = nullptr;

public:
    AdultGhost_Chase(AdultGhostController* _adultGhost) : IAdultGhostState(_adultGhost) {}
    ~AdultGhost_Chase() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;
};