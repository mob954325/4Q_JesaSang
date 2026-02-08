#include "PauseManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "EngineSystem/SceneSystem.h"
#include "System/InputSystem.h"
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


    PauseOff();
}

void PauseManager::OnUpdate(float delta)
{
    if (Input::GetKeyDown(pause_key))
    {
        // 미니게임중이라면 return

        // 일시정지 / 재개
        if (isPauseOn)
            PauseOff();
        else
            PauseOn();
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

void PauseManager::PauseOn()
{
    pausePannel->SetActive(true);
    isPauseOn = true;
}

void PauseManager::PauseOff()
{
    pausePannel->SetActive(false);
    isPauseOn = false;
}
