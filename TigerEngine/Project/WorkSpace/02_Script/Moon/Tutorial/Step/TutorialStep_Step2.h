#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step2 ]
// 현관에서 큰 소리 발생
// 
// 1. Sound 활성화 -> 현관 쪽에서 쿵! 큰 소리 발생
// 2. 대화창 ON : 독백 출력 “..!! 엄마가 왔나?”
// 3. [F] / [Space] / [클릭] / 방향키 중 하나의 입력 : 대화창 OFF 
// 4. Step3 전환 
// -----------------------------------------------------------

enum class Step2Phase
{
    Sound,      // 1. 큰 소리 연출
    Monologue,  // 2. 독백 출력
    WaitInput,  // 3. 입력 대기
    Done
};

class TutorialStep_Step2 : public ITutorialStep
{
private:
    bool isDone = false;

    float step2Timer = 0.0f;
    float delayStep2 = 2.0f;

    Step2Phase phase = Step2Phase::Sound;

public:
    TutorialStep_Step2(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step2() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};