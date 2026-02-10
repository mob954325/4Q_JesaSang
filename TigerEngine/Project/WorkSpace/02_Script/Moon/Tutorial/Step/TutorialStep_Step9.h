#pragma once
#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step9 ]
// 
// 
// 1. 
// 2. 
// 3. 
// 4. 
// -----------------------------------------------------------

enum class Step9Phase
{
    First,
    Second,
    Done
};

class TutorialStep_Step9 : public ITutorialStep
{
private:
    bool isDone = false;

    float stepTimer = 0.0f;
    float delayStart = 1.0f;

    Step9Phase phase = Step9Phase::First;

public:
    TutorialStep_Step9(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step9() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};