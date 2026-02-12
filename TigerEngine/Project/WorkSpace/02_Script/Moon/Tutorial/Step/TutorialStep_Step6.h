#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step6 ]
// 조상 유령 퇴장 후 장롱에서 나옴. 대사 출력
// 
// 2. 대사 출력 
// 3. [F] / [Space] / [클릭] / 방향키 중 하나의 입력 : 대화창 OFF 
// 4. Step7 전환 
// -----------------------------------------------------------

enum class Step6Phase
{
    Monologue1,
    WaitInput1,
    Monologue2,
    WaitInput2,
    Monologue3,
    WaitInput3,
    Done
};

class TutorialStep_Step6 : public ITutorialStep
{
private:
    bool isDone = false;

    float stepTimer = 0.0f;
    float delayStart = 1.0f;
    bool waitingStart = true;

    Step6Phase phase = Step6Phase::Monologue1;

    void Monologue1();
    void WaitInput1();
    void Monologue2();
    void WaitInput2();
    void Monologue3();
    void WaitInput3();

public:
    TutorialStep_Step6(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step6() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};