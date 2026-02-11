#include "WinPanel.h"
#include "../../../Engine/Object/GameObject.h"
#include "../../../Engine/EngineSystem/SceneSystem.h"
#include "../../../Engine/Util/ComponentAutoRegister.h"
#include "../../../Engine/Util/JsonHelper.h"
#include "../../../Engine/Util/PrefabUtil.h"
#include "../../../Engine/Manager/UIManager.h"
#include "Manager/WorldManager.h"

#include <algorithm>

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
    effectsPath.resize(80);
    for (int i = 0; i < 80; ++i)
    
        if(i < 10)
            effectsPath[i] = "..\\Assets\\Resource\\Effect\\Ending\\happy\\main0" + std::to_string(i) + ".png";
        else
            effectsPath[i] = "..\\Assets\\Resource\\Effect\\Ending\\happy\\main" + std::to_string(i) + ".png";
}

void WinPanel::OnStart()
{
    cutImg = GetOwner()->GetComponent<Image>();
}

void WinPanel::OnUpdate(float delta)
{
    if (notified && !isPlayed)
    {
        if (firstWaitTime < firstWaitMaxTime)
        {
            firstWaitTime += delta;
        }
        else
        {
            UpdateWorldSetting(delta);
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
}

void WinPanel::UpdateWorldSetting(float dt)
{
    if (!notified)
        return;

    auto& postProcessData = WorldManager::Instance().postProcessData;

    if (!worldSettingInited)
    {
        worldSettingInited = true;
        worldSettingTimer = 0.0f;

        baseExposure = postProcessData.exposure;
        baseUseBloom = postProcessData.useBloom;
        baseBloomIntensity = postProcessData.bloom_intensity;
        baseBloomThreshold = postProcessData.bloom_threshold;
        baseBloomScatter = postProcessData.bloom_scatter;
        baseBloomClamp = postProcessData.bloom_clamp;
        baseBloomTint = postProcessData.bloom_tint;
    }

    constexpr float kDuration = 2.0f;
    worldSettingTimer = std::min(worldSettingTimer + dt, kDuration);

    float t = worldSettingTimer / kDuration;
    t = std::clamp(t, 0.0f, 1.0f);
    const float smoothT = t * t * (3.0f - 2.0f * t); // smoothstep(0~1)

    const float targetExposure = baseExposure + 2.0f; // 0 -> +2 (pow2로 4배 밝아짐)
    const float targetBloomIntensity = std::max(baseBloomIntensity, 1.6f);
    constexpr float targetBloomThreshold = 3.65f;
    constexpr float targetBloomScatter = 0.7f;

    auto lerp = [](float a, float b, float w) { return a + (b - a) * w; };

    postProcessData.exposure = lerp(baseExposure, targetExposure, smoothT);

    // 승천 느낌: bloom을 켜고 강도/threshold를 램프업
    postProcessData.useBloom = true;
    postProcessData.bloom_intensity = lerp(baseBloomIntensity, targetBloomIntensity, smoothT);
    postProcessData.bloom_threshold = lerp(baseBloomThreshold, targetBloomThreshold, smoothT);
    postProcessData.bloom_scatter = lerp(baseBloomScatter, targetBloomScatter, smoothT);
    postProcessData.bloom_clamp = baseBloomClamp;
    postProcessData.bloom_tint = { 1.0f, 1.0f, 1.0f };
}

void WinPanel::Play()
{
    GetOwner()->SetActive(true);
    notified = true;

    worldSettingInited = false;
    worldSettingTimer = 0.0f;
}

nlohmann::json WinPanel::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void WinPanel::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}
