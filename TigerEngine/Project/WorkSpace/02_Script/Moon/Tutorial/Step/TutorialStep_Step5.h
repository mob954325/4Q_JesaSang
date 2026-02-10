#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step5 ]
// 
// 
// 1. 
// 2. 
// 3. 
// 4. 
// -----------------------------------------------------------

enum class Step5Phase
{
    First,
    Second,
    Done
};

class TutorialStep_Step5 : public ITutorialStep
{
private:
    bool isDone = false;

    float step5Timer = 0.0f;
    float delayStep5 = 1.0f;

    Step5Phase phase = Step5Phase::First;

public:
    TutorialStep_Step5(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step5() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};