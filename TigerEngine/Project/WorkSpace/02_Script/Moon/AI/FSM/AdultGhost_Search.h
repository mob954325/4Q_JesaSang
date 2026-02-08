#pragma once
#include "IAdultGhostState.h"

// -----------------------------------------------------------
// [ 탐색 Search ]
// 3~5 초 동안 플레이어 찾는 상태 
// 
//  - (if) Patrol 에서 기척 or 함정 감지로 넘어온 경우
//      3초 정지 하고 → 소리난 곳으로 이동 → 3~5초간 회전하며 대기
// 
//  - (if) Chase 에서 추격 실패로 넘어온 경우
//      마지막으로 플레이어 목격 했던 지점으로 이동 후 → 3~5초간 회전하며 대기
// 
// - 탐색 성공 (Search → Chase)  : 플레이어가 범위 이내 진입 ⇒ 시야 감지 
// - 탐색 실패 (Search → Return) : 탐색 3~5초 동안 플레이어 찾기 실패 
// -----------------------------------------------------------

enum class SearchPhase
{
    WaitBeforeMove,   // 3초 정지
    MoveToPoint,      // 타겟 이동
    RotateSearch      // 3~5초 회전
};

class AdultGhost_Search : public IAdultGhostState
{
private:
    SearchPhase phase = SearchPhase::WaitBeforeMove;

    float waitTimer = 0.0f;
    float rotateTimer = 0.0f;

    float waitTime = 3.0f;
    float rotateTime = 4.0f; 

    float baseYaw = 0.f;     // 회전 시작 기준
    float targetYaw = 0.f;  // 목표 회전값

    bool arrived = false;

public:
    AdultGhost_Search(AdultGhostController* _adultGhost) : IAdultGhostState(_adultGhost, AdultGhostState::Search) {}
    ~AdultGhost_Search() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;
};