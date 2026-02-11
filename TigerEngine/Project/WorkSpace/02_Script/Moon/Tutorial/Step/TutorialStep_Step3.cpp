#include "TutorialStep_Step3.h"

#include "System/InputSystem.h"


void TutorialStep_Step3::Enter()
{
    isDone = false;
    step3Timer = 0.0f;
    phase = Step3Phase::BabyInCome;



    std::cout << "[Step3] Enter" << std::endl;
}

void TutorialStep_Step3::Update(float deltaTime)
{
    step3Timer += deltaTime;

    switch (phase)
    {
    case Step3Phase::BabyInCome:

        if (step3Timer >= delayStep3)
        {
            BabyInCome();

            phase = Step3Phase::TrunToPlayer;
        }
        break;


    case Step3Phase::TrunToPlayer:
        phase = Step3Phase::Surprised;
        break;

    case Step3Phase::Surprised:
        phase = Step3Phase::Done;
        break;


    case Step3Phase::Done:
        isDone = true;
        break;
    }
}

bool TutorialStep_Step3::IsComplete()
{
    return isDone;
}

void TutorialStep_Step3::Exit()
{
    std::cout << "[Step3] Exit " << std::endl;
}


// ----------------------------------------------------------------------------

// 1. 아기 유령이 방으로 들어옴
//      - Move 애니메이션 기본 재생 
//      - 임시 위치 A, B 두고 이동하도록 하기 
void TutorialStep_Step3::BabyInCome()
{

}


// 2. 플레이어 쪽을 쳐다봄
//      - 플레이어가 있는 방향으로 회전
void TutorialStep_Step3::TrunToPlayer()
{

}


// 3. (!) 느낌표 오브젝트 활성화  
//      - 느낌표 애니메이션 재생(loop X) & 귀신은 살짝 움찔 
void TutorialStep_Step3::Surprised()
{

}


// 4. 아기 유령은 울면서 밖으로 퇴장 
//      - Cry 애니메이션 재생 
//      - 임시 위치 B 로 다시 이동 
void TutorialStep_Step3::Leave()
{

}