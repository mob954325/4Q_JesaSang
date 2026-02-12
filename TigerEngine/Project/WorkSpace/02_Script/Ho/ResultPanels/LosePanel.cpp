#include "LosePanel.h"
#include "../../../Engine/Object/GameObject.h"
#include "../../../Engine/EngineSystem/SceneSystem.h"
#include "../../../Engine/Util/ComponentAutoRegister.h"
#include "../../../Engine/Util/JsonHelper.h"
#include "Manager/WorldManager.h"

REGISTER_COMPONENT(LosePanel);

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<LosePanel>("LosePanel")
        .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr); 
}

void LosePanel::OnInitialize()
{
    effectsPath.resize(9);
    for (int i = 1; i < 10; i++)
    {
        effectsPath[i - 1] = "..\\Assets\\Resource\\Ending\\bad\\main0" + to_string(i) + ".png";
    }
}

void LosePanel::OnStart()
{
    cutImg = GetOwner()->GetComponent<Image>();
}

void LosePanel::OnUpdate(float delta)
{
    if (notified && !isPlayed)
    {
        timer += delta;
        //UpdateWorldSetting(delta);

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

void LosePanel::Play()
{
    GetOwner()->SetActive(true);
    notified = true;
}

nlohmann::json LosePanel::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void LosePanel::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void LosePanel::UpdateWorldSetting(float dt)
{
    auto& postProcessData = WorldManager::Instance().postProcessData;

    fadeOutTimer += dt;
    postProcessData.exposure = std::lerp(0.0f, -10.0f, fadeOutTimer / fadeOutMaxTime);
}