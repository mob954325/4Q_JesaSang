#include "TutorialStep_Step4.h"

#include "System/InputSystem.h"


void TutorialStep_Step4::Enter()
{
    isDone = false;
    step4Timer = 0.0f;
    phase = Step4Phase::AdultInCome;

    std::cout << "[Step4] Enter" << std::endl;
}

void TutorialStep_Step4::Update(float deltaTime)
{
    step4Timer += deltaTime;

    switch (phase)
    {
    case Step4Phase::AdultInCome:

        phase = Step4Phase::inQuest;
        break;


    case Step4Phase::inQuest:
        phase = Step4Phase::StartHide;
        break;


    case Step4Phase::StartHide:
        phase = Step4Phase::Fail;
        break;


    case Step4Phase::Fail:
        phase = Step4Phase::Clear;
        break;


    case Step4Phase::Clear:
        isDone = true;
        break;
    }
}

bool TutorialStep_Step4::IsComplete()
{
    return isDone;
}

void TutorialStep_Step4::Exit()
{
    std::cout << "[Step4] Exit " << std::endl;
}



// ------------------------------------------------------------------


// 1. 조상 유령 접근 + 거리에 따라서 화면 Frozen 
void TutorialStep_Step4::AdultInCome()
{

}


// 2. 퀘스트 활성화 "서둘러 장롱에 숨으세요" 3초 동안만 보여주고 다음 단계로 ㄱㄱ 
void TutorialStep_Step4::inQuest()
{

}


// 3. - 플레이어 조작 가능해짐 
//    - 조상 유령이 접근하기 전에 숨기 성공 => Step5로 전환
//      (숨으면 플레이어 렌더러 끄기)
//    - 조상 유령이 접근해서 플레이어와 닿으면 => 화면 어두워지면서 => Step4 재시작 
void TutorialStep_Step4::StartHide()
{

}