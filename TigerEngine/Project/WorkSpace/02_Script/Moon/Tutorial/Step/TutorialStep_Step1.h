#pragma once
#include "ITutorialStep.h"

class TutorialStep_Step1 : public ITutorialStep
{
public:
    TutorialStep_Step1(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step1() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};