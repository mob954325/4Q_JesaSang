#pragma once

#include "Object/Component.h"
#include "..\\..\\..\\Externals\\AudioModule_FMOD\\include\\AudioSource.h"

class AudioManagerComponent : public Component
{
    RTTR_ENABLE(Component)
public:
    AudioManagerComponent() { SetName("AudioManagerComponent"); }
    ~AudioManagerComponent() override = default;

    void OnInitialize() override;
    void OnUpdate(float delta) override;
    void OnDestory() override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    std::string bgmClipId = "BGM_Main";
    std::string ambClipId = "AMB_Wind";
    std::string bgmGroup = "bgm";
    std::string ambGroup = "amb";

    bool playOnEnter = true;
    bool loopBgm = true;
    bool loopAmb = true;
    float bgmVolume = 1.0f;
    float ambVolume = 1.0f;

private:
    void EnsureInitialized();
    void ApplySettings();

    AudioSource m_Bgm{};
    AudioSource m_Amb{};
    bool m_Initialized = false;
    bool m_Dirty = true;
    bool m_HasStarted = false;
    bool m_WasPaused = false;

    std::string m_LastBgmClipId{};
    std::string m_LastAmbClipId{};
    std::string m_LastBgmGroup{};
    std::string m_LastAmbGroup{};
    bool m_LastLoopBgm = true;
    bool m_LastLoopAmb = true;
    float m_LastBgmVolume = 1.0f;
    float m_LastAmbVolume = 1.0f;
    bool m_LastPlayOnEnter = true;
};
