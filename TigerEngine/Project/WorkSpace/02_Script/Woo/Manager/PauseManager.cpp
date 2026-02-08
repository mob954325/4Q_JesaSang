#include "PauseManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "EngineSystem/SceneSystem.h"
#include "System/InputSystem.h"
#include "System/TimeSystem.h"
#include "Object/GameObject.h"
#include "Components/RectTransform.h"
#include "Components/UI/Image.h"


REGISTER_COMPONENT(PauseManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<PauseManager>("PauseManager")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}


void PauseManager::OnStart()
{
    pausePannel = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("PausePannel");

    if (!pausePannel)
        cout << "[PauseManager] Missing ui objects!" << endl;

    // init
    pausePannel->SetActive(false);
    isPause = false;
}

void PauseManager::OnUpdate(float delta)
{
    if (Input::GetKeyDown(pause_key))
    {
        // TODO :: 다른곳에서 esc 누르는 경우 예외처리
        // 1. 미니게임 실행중이면 return
        TogglePause();
    }
}

nlohmann::json PauseManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void PauseManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void PauseManager::TogglePause()
{
    if (isPause) Resume();
    else Pause();
}

void PauseManager::Resume()
{
    pausePannel->SetActive(false);
    isPause = false;

    // 게임 시간 재개
    GameTimer::Instance().SetTimeScale(1.0f);
}

void PauseManager::Pause()
{
    pausePannel->SetActive(true);
    isPause = true;

    // 게임 시간 정지
    GameTimer::Instance().SetTimeScale(0.0f);
}
