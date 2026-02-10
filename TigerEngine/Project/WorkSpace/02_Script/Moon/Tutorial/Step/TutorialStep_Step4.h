#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step4 ]
// 
// 
// 1. 
// 2. 
// 3. 
// 4. 
// -----------------------------------------------------------

enum class Step4Phase
{
    First,
    Second,
    Done
};

class TutorialStep_Step4 : public ITutorialStep
{
private:
    bool isDone = false;

    float step4Timer = 0.0f;
    float delayStep4 = 1.0f;

    Step4Phase phase = Step4Phase::First;

public:
    TutorialStep_Step4(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step4() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};