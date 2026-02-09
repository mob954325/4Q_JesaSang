#pragma once
#include "IBabyGhostState.h"

// -----------------------------------------------------------
// [ 순찰 Patrol (Default) ]
// Grid를 통해 이동 가능 경로를 따라 순찰 + 목적지에 도착하면 잠시 멈췄다가 다시 순찰
// 
// - 시야 감지 (Patrol → Cry)  : 플레이어가 범위 이내 진입
// - 기척 감지 (Patrol → Search) : 시야 밖에서 기척(발소리 / 음식냄새) 감지
// - 함정 감지 (Patrol → Search) : 시야 밖에서 함정 파동 감지
// -----------------------------------------------------------

class BabyGhost_Patrol : public IBabyGhostState
{
private:
    AgentComponent* agent = nullptr;

private:
    // 감지 헬퍼
    void UpdateHideObjectVision();

public:
    BabyGhost_Patrol(BabyGhostController* _babyGhost) : IBabyGhostState(_babyGhost, BabyGhostState::Patrol) {}
    ~BabyGhost_Patrol() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;
};