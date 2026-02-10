#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step7 ]
// 
// 
// 1. 
// 2. 
// 3. 
// 4. 
// -----------------------------------------------------------

enum class Step7Phase
{
    First,
    Second,
    Done
};

class TutorialStep_Step7 : public ITutorialStep
{
private:
    bool isDone = false;

    float stepTimer = 0.0f;
    float delayStart = 1.0f;

    Step7Phase phase = Step7Phase::First;

public:
    TutorialStep_Step7(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step7() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};