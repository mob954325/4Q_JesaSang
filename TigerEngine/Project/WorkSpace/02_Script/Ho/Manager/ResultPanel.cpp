#include "ResultPanel.h"
#include "../../../Engine/EngineSystem/SceneSystem.h"
#include "../../../Engine/Object/GameObject.h"
#include "../../../Base/System/InputSystem.h"
#include "../../../Engine/Util/ComponentAutoRegister.h"
#include "../../../Engine/Util/JsonHelper.h"

REGISTER_COMPONENT(ResultPanel);

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<ResultPanel>("ResultPanel")
        .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void ResultPanel::OnInitialize()
{
}

void ResultPanel::OnStart()
{
    winPanel = SceneUtil::GetObjectByName("WinPanel")->GetComponent<WinPanel>();
    losePanel = SceneUtil::GetObjectByName("LosePanel")->GetComponent<LosePanel>();

    if (winPanel)
    {
        winPanel->GetOwner()->SetActive(false);
    }

    if (losePanel)
    {
        losePanel->GetOwner()->SetActive(false);
    }
}

void ResultPanel::OnUpdate(float dt)
{
    if (Input::GetKeyDown(DirectX::Keyboard::N))
    {
        losePanel->Play();
    }
    else if (Input::GetKeyDown(DirectX::Keyboard::M))
    {
        winPanel->Play();
    }
}

nlohmann::json ResultPanel::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void ResultPanel::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}