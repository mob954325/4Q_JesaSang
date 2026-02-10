#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step8 ]
// 
// 
// 1. 
// 2. 
// 3. 
// 4. 
// -----------------------------------------------------------

enum class Step8Phase
{
    First,
    Second,
    Done
};

class TutorialStep_Step8 : public ITutorialStep
{
private:
    bool isDone = false;

    float stepTimer = 0.0f;
    float delayStart = 1.0f;

    Step8Phase phase = Step8Phase::First;

public:
    TutorialStep_Step8(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step8() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};