#pragma once
#include "IAdultGhostState.h"

// -----------------------------------------------------------
// [ 공격 Attack ]
// 3초 딜레이(공격 모션) 이후 변환  
// 
// - 딜레이 이후 시야 감지 성공(공격 → 추격)
// - 딜레이 이후 시야 감지 성공(공격 → 탐색)
// -----------------------------------------------------------

class AdultGhost_Attack : public IAdultGhostState
{
private:
    float attackTimer = 0.0f;
    const float attackDelay = 3.0f;
    bool didDamage = false;

public:
    AdultGhost_Attack(AdultGhostController* _adultGhost) : IAdultGhostState(_adultGhost, AdultGhostState::Attack) {}
    ~AdultGhost_Attack() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;
};

