#include "WinPanel.h"
#include "../../../Engine/Object/GameObject.h"
#include "../../../Engine/EngineSystem/SceneSystem.h"
#include "../../../Engine/Util/ComponentAutoRegister.h"
#include "../../../Engine/Util/JsonHelper.h"
#include "../../../Engine/Util/PrefabUtil.h"
#include "../../../Engine/Manager/UIManager.h"

REGISTER_COMPONENT(WinPanel);

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<WinPanel>("WinPanel")
        .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void WinPanel::OnInitialize()
{
    effectsPath.resize(5);
    effectsPath[0] = "..\\Assets\\Resource\\Effect\\Freezing\\Freezing_effect01.png";
    effectsPath[1] = "..\\Assets\\Resource\\Effect\\Freezing\\Freezing_effect02.png";
    effectsPath[2] = "..\\Assets\\Resource\\Effect\\Freezing\\Freezing_effect03.png";
    effectsPath[3] = "..\\Assets\\Resource\\Effect\\Freezing\\Freezing_effect04.png";
    effectsPath[4] = "..\\Assets\\Resource\\Effect\\Freezing\\Freezing_effect05.png";
}

void WinPanel::OnStart()
{
    cutImg = GetOwner()->GetComponent<Image>();
}

void WinPanel::OnUpdate(float delta)
{
    if (notified && !isPlayed)
    {
        timer += delta;

        if (timer > maxTimer)
        {
            timer = 0.0f;

            if (index < effectsPath.size() - 1) // 1.. 2..
            {
                index++;
                if (cutImg)
                    cutImg->ChangeData(effectsPath[index]);
            }
            else
            {
                if (cutImg)
                    cutImg->ChangeData(cutPath);
                isPlayed = true;
            }
        }
    }
}

void WinPanel::UpdateWorldSetting(float dt)
{
}

void WinPanel::Play()
{
    GetOwner()->SetActive(true);
    notified = true;
}

nlohmann::json WinPanel::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void WinPanel::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}