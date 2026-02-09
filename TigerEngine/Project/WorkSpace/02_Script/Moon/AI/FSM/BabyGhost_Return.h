#pragma once
#include "IBabyGhostState.h"

// -----------------------------------------------------------
// [ 복귀 Return ]
// 웨이 포인트(해당 귀신 AI가 처음에 배치되어있던 위치)로 돌아가는 상태
// 
// - 시야 감지 (Return → Cry) : 플레이어가 시야 범위 이내 진입 
// - 복귀 완료 (Return → Patrol) : 웨이 포인트 도착 
// -----------------------------------------------------------

class BabyGhost_Return : public IBabyGhostState
{
public:
    BabyGhost_Return(BabyGhostController* _babyGhost) : IBabyGhostState(_babyGhost, BabyGhostState::Return) {}
    ~BabyGhost_Return() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;
};