#include "SoundManager.h"
#include "../../../Engine/Object/GameObject.h"
#include "../../../Engine/Util/ComponentAutoRegister.h"

REGISTER_COMPONENT(SoundManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<SoundManager>("SoundManager")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

void SoundManager::OnInitialize()
{
}

void SoundManager::OnStart()
{
    auto comps = GetOwner()->GetComponents(); 
    for (auto e : comps)
    {
        if (e->GetName() == "AudioSourceComponent")
        {
            sfxSources.push_back(static_cast<AudioSourceComponent*>(e));
        }
    }
}

void SoundManager::PlayBGM(BGMType type, bool restart)
{
    std::string id = ToString(type);

    bgmSource->SetLoop(restart);
    bgmSource->SetClipId(id);
    bgmSource->Play();
}

void SoundManager::StopBGM()
{
    bgmSource->Stop();
}

void SoundManager::PauseBGM(bool paused)
{
    bgmSource->Pause(paused);
}

void SoundManager::PlaySFX(SFXType type)
{
    auto id = ToString(type);

    // 비어있는 소스 찾기
    AudioSourceComponent* freeSrc = nullptr;
    for (auto* src : sfxSources)
    {
        if (src && !src->IsPlaying())
        {
            freeSrc = src;
            break;
        }
    }

    freeSrc->SetLoop(false);
    freeSrc->SetClipId(id);
    freeSrc->Play();
}

std::string SoundManager::ToString(BGMType type)
{
    switch (type)
    {
    case BGMType::Main:        return "BGM_Main";
    case BGMType::InGame_BG:   return "BGM_InGame_BG";
    case BGMType::Minigmae_BG: return "BGM_Minigmae_BG";
    }
    return "BGM_Unknown"; // enum 값 추가/손상 대비
}

std::string SoundManager::ToString(SFXType type)
{
    switch (type)
    {
    case SFXType::HiddenObj_Playerin_Sound:           return "SFX_HiddenObj_Playerin_Sound";
    case SFXType::HiddenObj_Playerout_Sound:          return "SFX_HiddenObj_Playerout_Sound";
    case SFXType::HiddenObj_Nointeraction_Sound:      return "SFX_HiddenObj_Nointeraction_Sound";
    case SFXType::FindObj_Interaction_1_Sound:        return "SFX_FindObj_Interaction_1_Sound";
    case SFXType::FindObj_Interaction_2_Sound:        return "SFX_FindObj_Interaction_2_Sound";
    case SFXType::FindObj_Acquiremap_Sound:           return "SFX_FindObj_Acquiremap_Sound";
    case SFXType::FindObj_Acquireitem_Sound:          return "SFX_FindObj_Acquireitem_Sound";
    case SFXType::PitfallObj_Floor_Sound:             return "SFX_PitfallObj_Floor_Sound";
    case SFXType::PitfallObj_Table_Sound:             return "SFX_PitfallObj_Table_Sound";
    case SFXType::PitfallObj_SleepGhost_Sound:        return "SFX_PitfallObj_SleepGhost_Sound";
    case SFXType::PitfallObj_SleepGhost_Wakeup_Sound: return "SFX_PitfallObj_SleepGhost_Wakeup_Sound";
    case SFXType::GoalObj_Sound:                      return "SFX_GoalObj_Sound";
    }
    return "SFX_Unknown";
}
