#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step5 ] 
// 조상 유령이 방에 들어옴
// 
// 1. 조상 유령이 방에 들어옴
// 2. 두리번 거림 
// 3. 대사 출력 
// 4. [F] / [Space] / [클릭] / 방향키 중 하나의 입력 : 대화창 OFF 
// 5. 유령이 밖으로 완전히 나감 
// 6. Step6 전환 
// -----------------------------------------------------------

enum class Step5Phase
{
    AdultInCome,
    LookAround,
    Monologue,
    WaitInput,
    AdultOut,
    Done
};

class TutorialStep_Step5 : public ITutorialStep
{
private:
    bool isDone = false;

    float step5Timer = 0.0f;
    float delayStep5 = 1.0f;

    Step5Phase phase = Step5Phase::AdultInCome;

    void AdultInCome();
    void LookAround();
    void Monologue();
    void WaitInput();
    void AdultOut();

public:
    TutorialStep_Step5(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step5() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};