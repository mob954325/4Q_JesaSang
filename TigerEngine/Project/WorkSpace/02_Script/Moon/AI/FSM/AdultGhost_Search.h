#pragma once
#include "IAdultGhostState.h"

class AdultGhost_Search : public IAdultGhostState
{
private:
    AgentComponent* agent = nullptr;

    bool arrived = false;
    float searchTimer = 0.0f;        // 탐색 시작
    float forceSearchTime = 5.0f;    // 탐색 자동 종료 (임시)

    int targetCX = 0;
    int targetCY = 0;
    bool hasSearchTarget = false;

public:
    AdultGhost_Search(AdultGhostController* _adultGhost) : IAdultGhostState(_adultGhost) {}
    ~AdultGhost_Search() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;

    // 타겟 설정 
    void SetSearchTarget(int cx, int cy)
    {
        targetCX = cx;
        targetCY = cy;
        hasSearchTarget = true;
    }
};