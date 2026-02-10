#pragma once
#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step9 ]
// 마지막 연출만 재생하는 스텝 (플레이어 조작)
// 
// 1. 대사 출력 
// 2. [F] / [Space] / [클릭] / 방향키 중 하나의 입력 : 대화창 OFF 
// 3. 플레이어 복도로 이동 연출 
// -----------------------------------------------------------

enum class Step9Phase
{
    Monologue,
    WaitInput,
    PlayerOut,
    Done
};

class TutorialStep_Step9 : public ITutorialStep
{
private:
    bool isDone = false;

    float stepTimer = 0.0f;
    float delayStart = 1.0f;

    Step9Phase phase = Step9Phase::Monologue;

    void Monologue();
    void WaitInput();
    void PlayerOut();

public:
    TutorialStep_Step9(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step9() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};