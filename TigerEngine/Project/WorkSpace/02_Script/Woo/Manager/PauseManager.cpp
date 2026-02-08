#include "PauseManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "EngineSystem/SceneSystem.h"
#include "System/InputSystem.h"
#include "System/TimeSystem.h"
#include "Object/GameObject.h"
#include "Components/RectTransform.h"
#include "Components/UI/Image.h"

#include "../MiniGame/MiniGameManager.h"


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
    if (!Input::GetKeyDown(pause_key))
        return;

    // 미니게임 [ESC] 우선.
    // 미니게임 팝업이 다 내려가고 난 뒤 일시정지 패널 on 가능
    if (auto* mg = MiniGameManager::Instance())
    {
        if (mg->IsPopupHiding())
            return;
    }

    TogglePause();
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
