#pragma once
#include "IAdultGhostState.h"

class AdultGhost_Chase : public IAdultGhostState
{
private:
    float chaseTimer = 0.0f;        // 탐색 시작
    float forceChaseTime = 3.0f;

    AgentComponent* agent = nullptr;
    GameObject* player = nullptr;

public:
    AdultGhost_Chase(AdultGhostController* _adultGhost) : IAdultGhostState(_adultGhost) {}
    ~AdultGhost_Chase() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;
};