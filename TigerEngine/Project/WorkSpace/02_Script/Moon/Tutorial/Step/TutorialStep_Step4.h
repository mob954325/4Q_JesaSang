#pragma once
#include "ITutorialStep.h"
#include "Components/UI/Image.h"

// -----------------------------------------------------------
// [ Step4 ]
// 조상 유령 접근  
// -----------------------------------------------------------

enum class Step4Phase
{
    ShowQuest,
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

    // --- Frozen Effect ---
    Image* frozenImage = nullptr;

    //float len1 = 500.0f;
    //float len2 = 400.5f;
    //float len3 = 300.0f;
    //float len4 = 200.5f;
    //float len5 = 100.0f;
    float len1 = 1000.0f;
    float len2 = 400.5f;
    float len3 = 300.0f;
    float len4 = 200.5f;
    float len5 = 100.0f;

    int frozenLevel = 0;
    int nextFrozenLevel = 0;

    float frozenTimer = 0.0f;
    float frozenMaxTime = 0.5f;

    std::vector<std::string> frozenPaths;

    void UpdateFrozen(float dt);

    // --- fade out ---
    float failFadeTimer = 0.0f;
    float failFadeMaxTime = 3.0f;   // 3초 동안 어두워짐
    bool failStarted = false;


    // Quest
    GameObject* Quest = nullptr;

private:
    void AdultInComeA();
    void AdultInComeB();
    void AdultArrive();
    void FailUpdate(float dt);

public:
    TutorialStep_Step4(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step4() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};