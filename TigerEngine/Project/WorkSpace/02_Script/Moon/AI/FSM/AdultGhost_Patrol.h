#pragma once
#include "IAdultGhostState.h"

class AdultGhost_Patrol : public IAdultGhostState
{
private:
    AgentComponent* agent = nullptr;

public:
    AdultGhost_Patrol(AdultGhostController* _adultGhost) : IAdultGhostState(_adultGhost) {}
    ~AdultGhost_Patrol() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;
};