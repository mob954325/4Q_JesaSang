#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step6 ]
// 
// 
// 1. 
// 2. 
// 3. 
// 4. 
// -----------------------------------------------------------

enum class Step6Phase
{
    First,
    Second,
    Done
};

class TutorialStep_Step6 : public ITutorialStep
{
private:
    bool isDone = false;

    float stepTimer = 0.0f;
    float delayStart = 1.0f;

    Step6Phase phase = Step6Phase::First;

public:
    TutorialStep_Step6(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step6() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};