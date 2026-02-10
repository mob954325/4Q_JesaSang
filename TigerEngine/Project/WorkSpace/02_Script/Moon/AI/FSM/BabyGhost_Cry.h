#pragma once
#include "IBabyGhostState.h"
#include "../AdultGhostController.h"

// -----------------------------------------------------------
// [ 울기 Cry ]
// 제자리에서 우는 애니메이션 재생하는 상태 
// 해당 상태에 들어오면, 가장 가까운 어른 유령 중에서 Patrol 상태인 유령에게
// 지금 내가 울고있다는 신호를 보낸다 
// -> 어른 유령이 일정거리 이상 다가와서 3초 동안 달래주면 다시 순찰 상태로 복귀한다. 
// -> Patrol 상태인 어른 유령이 아예 없다면 계속 울고있어야함... 
// 
// - 달래기 받기 (울기 -> 복귀)
// -----------------------------------------------------------

class BabyGhost_Cry : public IBabyGhostState
{
private:
    float careTimer = 0.0f;
    const float careDelay = 3.0f;

    AdultGhostController* caringAdult = nullptr;
    bool adultArrived = false; // 어른 유령이 근처에 도착했는지

public:
    BabyGhost_Cry(BabyGhostController* _babyGhost) : IBabyGhostState(_babyGhost, BabyGhostState::Cry) {}
    ~BabyGhost_Cry() = default;

    void Enter() override;
    void ChangeStateLogic() override;
    void Update(float deltaTime) override;
    void FixedUpdate(float deltaTime) override;
    void Exit() override;
};