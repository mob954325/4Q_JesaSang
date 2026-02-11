#pragma once
#include "ITutorialStep.h"
#include "Components/UI/Image.h"

// -----------------------------------------------------------
// [ Step4 ]
// 조상 유령 접근  
// -----------------------------------------------------------

enum class Step4Phase
{
    AdultInComeA,
    AdultInComeB,
    AdultArrive,
    Fail, 
    Clear
};

class TutorialStep_Step4 : public ITutorialStep
{
private:
    bool isDone = false;

    float step4Timer = 0.0f;
    float delayStep4 = 1.0f;

    Step4Phase phase = Step4Phase::AdultInComeA;

    GameObject* targetA = nullptr;
    GameObject* targetB = nullptr;
    GameObject* hideObject = nullptr;

    Transform* adultTranform = nullptr;

    bool phaseStarted = false;

    float speed = 120.0f;
    float fast_speed = 200.0;

private:
    void AdultInComeA();
    void AdultInComeB();
    void AdultArrive();

public:
    TutorialStep_Step4(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step4() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};