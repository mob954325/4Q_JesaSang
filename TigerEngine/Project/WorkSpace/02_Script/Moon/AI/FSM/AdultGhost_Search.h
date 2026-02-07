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

class AdultGhost_Search : public IAdultGhostState
{
private:
    float searchTimer = 0.f;
    float maxSearchTime = 5.0f;

    bool arrived = false;

    int targetCX = 0;
    int targetCY = 0;
    bool hasSearchTarget = false; 

private:
    void TrySetInitialTarget();

public:
    AdultGhost_Search(AdultGhostController* _adultGhost) : IAdultGhostState(_adultGhost, AdultGhostState::Search) {}
    ~AdultGhost_Search() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;

    // 타겟 설정 
    void SetSearchTarget(int cx, int cy);
};