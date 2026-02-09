#pragma once
#include "IAdultGhostState.h"

// -----------------------------------------------------------
// [ 복귀 Return ]
// 웨이 포인트(해당 귀신 AI가 처음에 배치되어있던 위치)로 돌아가는 상태
// 
// - 시야 감지 (복귀 → 추격) : 플레이어가 범위 이내 진입 
// - 복귀 완료 (복귀 → 순찰) : 웨이 포인트 도착 
// -----------------------------------------------------------

class AdultGhost_Return : public IAdultGhostState
{
public:
    AdultGhost_Return(AdultGhostController* _adultGhost) : IAdultGhostState(_adultGhost, AdultGhostState::Patrol) {}
    ~AdultGhost_Return() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;
};