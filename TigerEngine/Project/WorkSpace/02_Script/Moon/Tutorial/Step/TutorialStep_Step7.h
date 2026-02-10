#pragma once
#include "ITutorialStep.h"

// -----------------------------------------------------------
// [ Step7 ]
// 제사상 연출 
// 
// 1. 플레이어는 제사상 앞으로 이동 (조작X, 연출O)
// 2. 카메라가 제사상 비췄다가 돌아와서 다시 플레이어 비추기 
//      (제사상 접시 비워져있는거 보여주기 위함)
// 3. 플레이어의 대사 4개 출력 : 넘기기 입력 O
// 4. 카메라 과일 바구니로 비췄다가 돌아오기
// 5. Step8 전환 
// -----------------------------------------------------------

enum class Step7Phase
{
    PlayerToJesasang,
    CameraToJesasang,
    Monologue,
    CameraToFruit,
    Done
};

class TutorialStep_Step7 : public ITutorialStep
{
private:
    bool isDone = false;

    float stepTimer = 0.0f;
    float delayStart = 1.0f;

    Step7Phase phase = Step7Phase::PlayerToJesasang;

    void PlayerToJesasang();
    void CameraToJesasang();
    void Monologue();
    void CameraToFruit();

public:
    TutorialStep_Step7(TutorialController* _tutorialController) : ITutorialStep(_tutorialController) {}
    ~TutorialStep_Step7() = default;

    void Enter() override;
    void Update(float deltaTime) override;
    bool IsComplete() override;
    void Exit() override;
};