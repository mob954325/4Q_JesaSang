#include "AudioManagerComponent.h"

#include "Manager/AudioManager.h"
#include "EngineSystem/PlayModeSystem.h"
#include "Util/ComponentAutoRegister.h"
#include "Util/JsonHelper.h"

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
    EnsureInitialized();
}

void AudioManagerComponent::OnUpdate(float delta)
{
    (void)delta;

    EnsureInitialized();
    ApplySettings();

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
    m_Initialized = false;
    m_Dirty = true;
}

nlohmann::json AudioManagerComponent::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void AudioManagerComponent::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
    m_Dirty = true;
}

void AudioManagerComponent::EnsureInitialized()
{
    if (m_Initialized)
    {
        return;
    }

    auto& system = AudioManager::Instance().GetSystem();
    if (auto* fmodSystem = system.GetSystem())
    {
        m_Bgm.Init(fmodSystem);
        m_Amb.Init(fmodSystem);
        m_Initialized = true;
    }
}

void AudioManagerComponent::ApplySettings()
{
    if (!m_Initialized)
    {
        return;
    }

    auto& system = AudioManager::Instance().GetSystem();

    const bool groupChanged = bgmGroup != m_LastBgmGroup || ambGroup != m_LastAmbGroup;
    const bool clipChanged = bgmClipId != m_LastBgmClipId || ambClipId != m_LastAmbClipId;
    const bool loopChanged = loopBgm != m_LastLoopBgm || loopAmb != m_LastLoopAmb;
    const bool volumeChanged = bgmVolume != m_LastBgmVolume || ambVolume != m_LastAmbVolume;

    if (!m_Dirty && !groupChanged && !clipChanged && !loopChanged && !volumeChanged)
    {
        return;
    }

    if (!bgmGroup.empty() && (m_Dirty || bgmGroup != m_LastBgmGroup))
    {
        m_Bgm.SetChannelGroup(system.GetOrCreateChannelGroup(bgmGroup));
        m_LastBgmGroup = bgmGroup;
    }
    if (!ambGroup.empty() && (m_Dirty || ambGroup != m_LastAmbGroup))
    {
        m_Amb.SetChannelGroup(system.GetOrCreateChannelGroup(ambGroup));
        m_LastAmbGroup = ambGroup;
    }
    if (!bgmClipId.empty() && (m_Dirty || bgmClipId != m_LastBgmClipId))
    {
        m_Bgm.SetClip(AudioManager::Instance().GetOrCreateClip(bgmClipId));
        m_LastBgmClipId = bgmClipId;
    }
    if (!ambClipId.empty() && (m_Dirty || ambClipId != m_LastAmbClipId))
    {
        m_Amb.SetClip(AudioManager::Instance().GetOrCreateClip(ambClipId));
        m_LastAmbClipId = ambClipId;
    }
    if (m_Dirty || loopBgm != m_LastLoopBgm)
    {
        m_Bgm.SetLoop(loopBgm);
        m_LastLoopBgm = loopBgm;
    }
    if (m_Dirty || loopAmb != m_LastLoopAmb)
    {
        m_Amb.SetLoop(loopAmb);
        m_LastLoopAmb = loopAmb;
    }
    if (m_Dirty || bgmVolume != m_LastBgmVolume)
    {
        m_Bgm.SetVolume(bgmVolume);
        m_LastBgmVolume = bgmVolume;
    }
    if (m_Dirty || ambVolume != m_LastAmbVolume)
    {
        m_Amb.SetVolume(ambVolume);
        m_LastAmbVolume = ambVolume;
    }

    m_Dirty = false;
}
