#include "AudioManagerComponent.h"

#include "Manager/AudioManager.h"
#include "EngineSystem/PlayModeSystem.h"
#include "Util/ComponentAutoRegister.h"

REGISTER_COMPONENT(AudioManagerComponent);

RTTR_REGISTRATION
{
    rttr::registration::class_<AudioManagerComponent>("AudioManagerComponent")
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr)
        .property("BgmClipId", &AudioManagerComponent::bgmClipId)
        .property("AmbClipId", &AudioManagerComponent::ambClipId)
        .property("BgmGroup", &AudioManagerComponent::bgmGroup)
        .property("AmbGroup", &AudioManagerComponent::ambGroup)
        .property("PlayOnEnter", &AudioManagerComponent::playOnEnter)
        .property("LoopBgm", &AudioManagerComponent::loopBgm)
        .property("LoopAmb", &AudioManagerComponent::loopAmb)
        .property("BgmVolume", &AudioManagerComponent::bgmVolume)
        .property("AmbVolume", &AudioManagerComponent::ambVolume);
}

void AudioManagerComponent::OnInitialize()
{
    auto& system = AudioManager::Instance().GetSystem();
    if (auto* fmodSystem = system.GetSystem())
    {
        m_Bgm.Init(fmodSystem);
        m_Amb.Init(fmodSystem);
    }
}

void AudioManagerComponent::OnUpdate(float delta)
{
    (void)delta;

    auto& system = AudioManager::Instance().GetSystem();

    // apply inspector changes live
    if (bgmGroup != m_LastBgmGroup && !bgmGroup.empty())
    {
        m_Bgm.SetChannelGroup(system.GetOrCreateChannelGroup(bgmGroup));
        m_LastBgmGroup = bgmGroup;
    }
    if (ambGroup != m_LastAmbGroup && !ambGroup.empty())
    {
        m_Amb.SetChannelGroup(system.GetOrCreateChannelGroup(ambGroup));
        m_LastAmbGroup = ambGroup;
    }
    if (bgmClipId != m_LastBgmClipId && !bgmClipId.empty())
    {
        m_Bgm.SetClip(AudioManager::Instance().GetOrCreateClip(bgmClipId));
        m_LastBgmClipId = bgmClipId;
    }
    if (ambClipId != m_LastAmbClipId && !ambClipId.empty())
    {
        m_Amb.SetClip(AudioManager::Instance().GetOrCreateClip(ambClipId));
        m_LastAmbClipId = ambClipId;
    }
    if (loopBgm != m_LastLoopBgm)
    {
        m_Bgm.SetLoop(loopBgm);
        m_LastLoopBgm = loopBgm;
    }
    if (loopAmb != m_LastLoopAmb)
    {
        m_Amb.SetLoop(loopAmb);
        m_LastLoopAmb = loopAmb;
    }
    if (bgmVolume != m_LastBgmVolume)
    {
        m_Bgm.SetVolume(bgmVolume);
        m_LastBgmVolume = bgmVolume;
    }
    if (ambVolume != m_LastAmbVolume)
    {
        m_Amb.SetVolume(ambVolume);
        m_LastAmbVolume = ambVolume;
    }

    PlayModeState curr = PlayModeSystem::Instance().GetPlayMode();

    if (m_LastPlayOnEnter != playOnEnter)
    {
        if (!playOnEnter)
        {
            m_Bgm.Stop();
            m_Amb.Stop();
            m_HasStarted = false;
            m_WasPaused = false;
        }
        m_LastPlayOnEnter = playOnEnter;
    }

    if (curr == PlayModeState::Playing)
    {
        if (!m_HasStarted && playOnEnter)
        {
            m_Bgm.Play(true);
            m_Amb.Play(true);
            m_HasStarted = true;
            m_WasPaused = false;
        }
        else if (m_WasPaused)
        {
            m_Bgm.Pause(false);
            m_Amb.Pause(false);
            m_WasPaused = false;
        }
    }
    else if (curr == PlayModeState::Paused)
    {
        if (!m_WasPaused)
        {
            m_Bgm.Pause(true);
            m_Amb.Pause(true);
            m_WasPaused = true;
        }
    }
    else if (curr == PlayModeState::Stopped)
    {
        m_Bgm.Stop();
        m_Amb.Stop();
        m_HasStarted = false;
        m_WasPaused = false;
    }
}

void AudioManagerComponent::OnDestory()
{
    m_Bgm.Stop();
    m_Amb.Stop();
    m_HasStarted = false;
    m_WasPaused = false;
}

nlohmann::json AudioManagerComponent::Serialize()
{
    nlohmann::json data;
    rttr::type t = rttr::type::get(*this);
    data["type"] = t.get_name().to_string();
    data["properties"] = nlohmann::json::object();
    data["properties"]["BgmClipId"] = bgmClipId;
    data["properties"]["AmbClipId"] = ambClipId;
    data["properties"]["BgmGroup"] = bgmGroup;
    data["properties"]["AmbGroup"] = ambGroup;
    data["properties"]["PlayOnEnter"] = playOnEnter;
    data["properties"]["LoopBgm"] = loopBgm;
    data["properties"]["LoopAmb"] = loopAmb;
    data["properties"]["BgmVolume"] = bgmVolume;
    data["properties"]["AmbVolume"] = ambVolume;
    return data;
}

void AudioManagerComponent::Deserialize(nlohmann::json data)
{
    if (!data.contains("properties"))
    {
        return;
    }

    const auto& props = data["properties"];
    if (props.contains("BgmClipId")) bgmClipId = props["BgmClipId"].get<std::string>();
    if (props.contains("AmbClipId")) ambClipId = props["AmbClipId"].get<std::string>();
    if (props.contains("BgmGroup")) bgmGroup = props["BgmGroup"].get<std::string>();
    if (props.contains("AmbGroup")) ambGroup = props["AmbGroup"].get<std::string>();
    if (props.contains("PlayOnEnter")) playOnEnter = props["PlayOnEnter"].get<bool>();
    if (props.contains("LoopBgm")) loopBgm = props["LoopBgm"].get<bool>();
    if (props.contains("LoopAmb")) loopAmb = props["LoopAmb"].get<bool>();
    if (props.contains("BgmVolume")) bgmVolume = props["BgmVolume"].get<float>();
    if (props.contains("AmbVolume")) ambVolume = props["AmbVolume"].get<float>();
}
