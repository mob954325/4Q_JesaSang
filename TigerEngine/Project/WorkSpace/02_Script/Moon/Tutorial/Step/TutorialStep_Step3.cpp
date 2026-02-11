#include "TutorialStep_Step3.h"

#include "System/InputSystem.h"
#include "System/TimeSystem.h"
#include "EngineSystem/SceneSystem.h"

void TutorialStep_Step3::Enter()
{
    isDone = false;
    step3Timer = 0.0f;
    phase = Step3Phase::BabyInCome;

    targetA = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Baby_Target_A");
    targetB = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("Baby_Target_B");

    babyTranform = tutorialController->babyGhost_Obj->GetTransform();

    exclamation = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("FX_Exclamation");
    if (exclamation) exclamation->SetActive(false);

    std::cout << "[Step3] Enter" << std::endl;
}

void TutorialStep_Step3::Update(float deltaTime)
{
    step3Timer += deltaTime;

    switch (phase)
    {
    case Step3Phase::BabyInCome:

        BabyInCome();
        break;


    case Step3Phase::TrunToPlayer:

        TrunToPlayer();
        break;


    case Step3Phase::Surprised:

        Surprised();
        break;

    case Step3Phase::Leave:

        Leave();
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
//      - Baby_Target_A로 이동
//      - Baby_Target_A에 도착하면 멈추고 다음 단계로 이동
void TutorialStep_Step3::BabyInCome()
{
    Vector3 target = targetA->GetTransform()->GetWorldPosition();
    Vector3 pos = tutorialController->babyGhost_Obj->GetTransform()->GetWorldPosition();
    Vector3 dir = target - pos;

    if (dir.Length() < 5.0f)
    {
        phase = Step3Phase::TrunToPlayer;
        return;
    }

    dir.Normalize();

    float speed = 60.f;
    float dt = GameTimer::Instance().UnscaledDeltaTime();

    babyTranform->Translate(dir * speed * dt);

    // 이동 방향 바라보게 회전
    float yaw = atan2f(-dir.x, -dir.z);
    babyTranform->SetRotationY(yaw);
}


// 2. 플레이어 쪽을 쳐다봄
//      - 플레이어가 있는 방향으로 회전
void TutorialStep_Step3::TrunToPlayer()
{
    Vector3 dir = tutorialController->player_Obj->GetTransform()->GetWorldPosition() - babyTranform->GetWorldPosition();
    dir.y = 0;
    dir.Normalize();

    float targetYaw = atan2f(-dir.x, -dir.z);
    float curYaw = babyTranform->GetYaw();
    float delta = targetYaw - curYaw;

    // 각도 래핑 (-pi ~ pi)
    while (delta > XM_PI)  delta -= XM_2PI;
    while (delta < -XM_PI) delta += XM_2PI;

    if (fabs(delta) < 0.05f)
    {
        babyTranform->SetRotationY(targetYaw);
        phase = Step3Phase::Surprised;
        return;
    }

    float turnSpeed = 4.0f;
    float dt = GameTimer::Instance().UnscaledDeltaTime();

    babyTranform->SetRotationY(curYaw + delta * turnSpeed * dt);
}


// 3. (!) 느낌표 오브젝트 활성화  
//      - 느낌표 애니메이션 재생(loop X) & 귀신은 살짝 움찔 
void TutorialStep_Step3::Surprised()
{
    if (!phaseStarted)
    {
        phaseStarted = true;

        if (exclamation) exclamation->SetActive(true);

        step3Timer = 0.0f;
    }

    step3Timer += GameTimer::Instance().UnscaledDeltaTime();

    if (step3Timer >= 2.0f)
    {
        if (exclamation) exclamation->SetActive(false);

        phase = Step3Phase::Leave;
        phaseStarted = false;
    }
}


// 4. 아기 유령은 울면서 밖으로 퇴장 
//      - Cry 애니메이션 재생 
//      - Baby_Target_B 로 다시 이동 
void TutorialStep_Step3::Leave()
{
    if (!phaseStarted)
    {
        phaseStarted = true;
        tutorialController->BabyGhost_animController->ChangeState("Cry");
    }

    Vector3 target = targetB->GetTransform()->GetWorldPosition();
    Vector3 pos = babyTranform->GetWorldPosition();
    Vector3 dir = target - pos;

    if (dir.Length() < 5.0f)
    {
        phase = Step3Phase::Done;
        return;
    }

    dir.Normalize();

    float speed = 60.f;
    float dt = GameTimer::Instance().UnscaledDeltaTime();

    babyTranform->Translate(dir * speed * dt);

    // 이동 방향 바라보게 회전
    float yaw = atan2f(-dir.x, -dir.z);
    babyTranform->SetRotationY(yaw);
}