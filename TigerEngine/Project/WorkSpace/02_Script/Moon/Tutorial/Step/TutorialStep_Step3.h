#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step3 ]
// 아기 유령 입장
// 
// 1. 아기 유령이 방으로 들어옴
// 2. 플레이어 쪽을 쳐다봄
// 3. (!) 느낌표 이펙트 잠깐 등장
// 4. 아기 유령은 울면서 밖으로 퇴장
// 5. Step4 전환
// -----------------------------------------------------------

enum class Step3Phase
{
    BabyInCome, 
    TrunToPlayer,
    Surprised,
    Leave,
    Done
};

class TutorialStep_Step3 : public ITutorialStep
{
private:
    bool isDone = false;

    float step3Timer = 0.0f;
    float delayStep3 = 1.0f;

    Step3Phase phase = Step3Phase::BabyInCome;

    GameObject* targetA = nullptr;
    GameObject* targetB = nullptr;
    GameObject* exclamation = nullptr;

    Transform* babyTranform = nullptr;

    bool phaseStarted = false;

    float speed = 120.0f;

private:
    void BabyInCome();
    void TrunToPlayer();
    void Surprised();
    void Leave();

public:
    TutorialStep_Step3(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step3() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};