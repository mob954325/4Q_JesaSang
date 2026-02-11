#include "TutorialStep_Step4.h"

#include "System/InputSystem.h"
#include "System/TimeSystem.h"

#include "EngineSystem/SceneSystem.h"

void TutorialStep_Step4::Enter()
{
    isDone = false;
    step4Timer = 0.0f;
    phase = Step4Phase::AdultInCome;

    hideGauge = 0.0f;

    auto scene = SceneSystem::Instance().GetCurrentScene();

    hideObject = scene->GetGameObjectByName("HideObject");

    image_sensorOn = scene->GetGameObjectByName("Image_SensorOn")->GetComponent<Image>();
    image_interactionOn = scene->GetGameObjectByName("Image_InteractionOn")->GetComponent<Image>();
    image_interactionGauge = scene->GetGameObjectByName("Image_InteractionGauge")->GetComponent<Image>();

    if (image_sensorOn) image_sensorOn->SetActive(false);
    if (image_interactionOn) image_interactionOn->SetActive(false);
    if (image_interactionGauge) image_interactionGauge->SetActive(false);

    std::cout << "[Step4] Enter" << std::endl;
}

void TutorialStep_Step4::Update(float deltaTime)
{
    step4Timer += deltaTime;

    switch (phase)
    {
    case Step4Phase::AdultInCome:
        AdultInCome();
        phase = Step4Phase::inQuest;
        break;


    case Step4Phase::inQuest:
        inQuest();
        phase = Step4Phase::StartHide;
        break;


    case Step4Phase::StartHide:
        StartHide();
        phase = Step4Phase::Fail;
        break;


    case Step4Phase::Fail:
        std::cout << "[Step4] Fail → Restart\n";
        phase = Step4Phase::AdultInCome;
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
//void TutorialStep_Step4::StartHide()
//{
//    // 플레이어 조작 가능해짐. 방향키로 이동, F키는 특정 오브젝트와 상호작용 
//
//    // HideObject 라는 이름의 오브젝트와 일정거리 가까워지면 
//    // Image_SensorOn 이라는 오브젝트가 활성화 되고,
//    // 더욱 더 가까워지면 Image_InteractionOn, Image_interactionGauge 오브젝트가 활성화.
//    // 
//    // 그 상태에서 F를 꾹 누르면 점점 SetFillAmount의 게이지가 채워지기 
//    // image_interactionGauge->SetFillAmount(progress);
//
//    // 게이지를 다 채우면 ChangeState(PlayerState_Tutorial::Hide) 로 플레이어 상태 Hide로변경 -> 성공 : phase = Step4Phase::Clear; 로 넘어가기
//
//    // 만약 다가오는 조상 유령과 부딪힌다면 => phase = Step4Phase::Fail
//    // 이 되고, Step4Phase의 AdultInCome 부터 (다시 처음부터 시작)
//
//}

void TutorialStep_Step4::StartHide()
{
    auto* player = tutorialController->player_Obj;
    if (!player || !hideObject) return;

    auto pPos = player->GetTransform()->GetWorldPosition();
    auto hPos = hideObject->GetTransform()->GetWorldPosition();

    float dist = Vector3::Distance(pPos, hPos);

    const float sensorDist = 350.0f;
    const float interactDist = 150.0f;

    // ---------------- UI 단계 ----------------

    if (dist <= sensorDist)
        image_sensorOn->SetActive(true);
    else
        image_sensorOn->SetActive(false);

    if (dist <= interactDist)
    {
        image_interactionOn->SetActive(true);
        image_interactionGauge->SetActive(true);
    }
    else
    {
        image_interactionOn->SetActive(false);
        image_interactionGauge->SetActive(false);
        hideGauge = 0.0f;
        image_interactionGauge->SetFillAmount(0.0f);
        return;
    }

    // ---------------- F 키 게이지 ----------------
    if (Input::GetKey(Keyboard::Keys::F))
    {
        hideGauge += hideFillSpeed * GameTimer::Instance().DeltaTime();
        hideGauge = std::min(hideGauge, 1.0f);
    }
    else
    {
        hideGauge -= hideFillSpeed * GameTimer::Instance().DeltaTime();
        hideGauge = std::max(hideGauge, 0.0f);
    }

    image_interactionGauge->SetFillAmount(hideGauge);

    // ---------------- 성공 ----------------
    if (hideGauge >= 1.0f)
    {
        tutorialController->player_Obj->SetActive(false); // 렌더 끄기
        phase = Step4Phase::Clear;
        return;
    }

    // ---------------- 실패 ----------------
    if (tutorialController->adultGhost_Obj)
    {
        float ghostDist = Vector3::Distance(
            tutorialController->adultGhost_Obj->GetTransform()->GetWorldPosition(),
            pPos);

        if (ghostDist < 50.0f)
        {
            //auto& pp = WorldManager::Instance().postProcessData;
            //pp.useVignette = true;
            //pp.vignette_smoothness = 1.0f;

            phase = Step4Phase::Fail;
        }
    }
}
