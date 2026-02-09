#pragma once
#include "IAdultGhostState.h"

// -----------------------------------------------------------
// [ 추격 Chase ]
// 플레이어 위치로 이동하는 상태 
// 
// - (if) Patrol에서 울기 감지로 넘어온 경우
//      1. 애기 유령이 울었던 지점으로 이동,
//      2. 도착하면 3~5초간 회전하며 대기
//      3. 플레이어 위치를 향해 5초간 이동(시야 밖이어도 ㄱㄴ)
//          3-1. 이후 시야에 감지되면    (추격 → 추격) : 추격 유지
//          3-2. 이후 시야에 감지 안되면 (추격 → 탐색) : 추격 실패
// 
// - 추격 성공 (Chase → Attack) : 플레이어와 충돌
// - 추격 실패 (Chase → Search) : 플레이어 놓침
// -----------------------------------------------------------

class AdultGhost_Chase : public IAdultGhostState
{
private:
    float chaseTimer = 0.0f;
    float minChaseTime = 3.0f;     // 최소 추격 유지 시간

    float repathTimer = 0.0f;
    float repathInterval = 0.3f;  // 목표 갱신 주기

    GameObject* target = nullptr;

private:
    void UpdateTargetGrid();
    bool CanGiveUpChase() const;

public:
    AdultGhost_Chase(AdultGhostController* _adultGhost) : IAdultGhostState(_adultGhost, AdultGhostState::Chase) {}
    ~AdultGhost_Chase() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;
};