#include "SoundManager.h"
#include "../../../Engine/Object/GameObject.h"
#include "../../../Engine/Util/ComponentAutoRegister.h"
#include "../../../Engine/Util/JsonHelper.h"

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

    bgmSource = nullptr;
    sfxSources.clear();

    for (auto* c : comps)
    {
        if (!c) continue;
        if (c->GetName() == "AudioSourceComponent")
        {
            auto* src = static_cast<AudioSourceComponent*>(c); 
            if (!bgmSource) bgmSource = src;      // 첫 오디오 소스를 BGM으로
            else sfxSources.push_back(src);       // 나머지 SFX로
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

    if (freeSrc)
    {
        freeSrc->SetLoop(false);
        freeSrc->SetClipId(id);
        freeSrc->Play();
    }
}

nlohmann::json SoundManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void SoundManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

std::string SoundManager::ToString(BGMType type)
{
    switch (type)
    {
    case BGMType::Main:        return "Main";
    case BGMType::InGame_BG:   return "InGame_BG";
    case BGMType::Minigmae_BG: return "Minigmae_BG";
    }
    return "BGM_Unknown"; // enum 값 추가/손상 대비
}

std::string SoundManager::ToString(SFXType type)
{
    switch (type)
    {
    case SFXType::HiddenObj_Playerin_Sound:           return "HiddenObj_Playerin_Sound";
    case SFXType::HiddenObj_Playerout_Sound:          return "HiddenObj_Playerout_Sound";
    case SFXType::HiddenObj_Nointeraction_Sound:      return "HiddenObj_Nointeraction_Sound";
    case SFXType::FindObj_Interaction_1_Sound:        return "FindObj_Interaction_1_Sound";
    case SFXType::FindObj_Interaction_2_Sound:        return "FindObj_Interaction_2_Sound";
    case SFXType::FindObj_Acquiremap_Sound:           return "FindObj_Acquiremap_Sound";
    case SFXType::FindObj_Acquireitem_Sound:          return "FindObj_Acquireitem_Sound";
    case SFXType::PitfallObj_Floor_Sound:             return "PitfallObj_Floor_Sound";
    case SFXType::PitfallObj_Table_Sound:             return "PitfallObj_Table_Sound";
    case SFXType::PitfallObj_SleepGhost_Sound:        return "PitfallObj_SleepGhost_Sound";
    case SFXType::PitfallObj_SleepGhost_Wakeup_Sound: return "PitfallObj_SleepGhost_Wakeup_Sound";
    case SFXType::GoalObj_Sound:                      return "GoalObj_Sound";
    }
    return "SFX_Unknown";
}
