#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step1 ]
// 오프닝 일러스트 종료 후, 방에서 깨어나는 단계 
// 
// 1. 눈을 깜빡이는 듯한 연출 (약 3초)
// 2. 대화창(다이얼로그) ON -> 독백 출력 
// 3. [F] / [Space] / [클릭] / 방향키 중 하나의 입력 : 대화창 OFF 
// 4. Step2 전환 
// -----------------------------------------------------------

class DialogueUIController;

enum class Step1Phase
{
    Blink,      // 1. 눈 깜빡 연출
    Monologue,  // 2. 독백 출력
    WaitInput,  // 3. 입력 대기
    Done        // 4. 종료
};

class TutorialStep_Step1 : public ITutorialStep
{
private:
    bool isDone = false;

    float step1Timer = 0.0f;
    float BlinkTime = 10.0f;
    float BlinkDelay = 2.0f;

    Step1Phase phase = Step1Phase::Blink;

private:
    void Blink();
    void Monologue();

private:
    GameObject* Blink_Top = nullptr;
    GameObject* Blink_Bottom = nullptr;
    DialogueUIController* dialogue = nullptr;

public:
    TutorialStep_Step1(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step1() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};