#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step4 ]
// 조상 유령 접근  
// 
// 1. 조상 유령 접근 + 화면 Frozen 
// 2. 퀘스트 활성화 "서둘러 장롱에 숨으세요"
// 3. 플레이어 조작 가능해짐 
//      - 조상 유령이 접근하기 전에 숨기 성공 => Step5로 전환
//      - 조상 유령이 접근해서 플레이어와 닿으면 => 화면 어두워지면서 => 다시 Step4 재시작 
// -----------------------------------------------------------

enum class Step4Phase
{
    AdultInCome,
    inQuest,
    StartHide,
    Fail,
    Clear
};

class TutorialStep_Step4 : public ITutorialStep
{
private:
    bool isDone = false;

    float step4Timer = 0.0f;
    float delayStep4 = 1.0f;

    Step4Phase phase = Step4Phase::AdultInCome;

private:
    void AdultInCome();
    void inQuest();
    void StartHide();

public:
    TutorialStep_Step4(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step4() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};