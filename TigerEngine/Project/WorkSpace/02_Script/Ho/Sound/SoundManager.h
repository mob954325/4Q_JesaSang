#pragma once
#include "../../../Base/System/Singleton.h"
#include "../../../Engine/Components/AudioSourceComponent.h"
#include "../../../Engine/Components/ScriptComponent.h"
#include <string>
#include <queue>

#include "Manager/AudioManager.h"

enum class BGMType
{
    Main,
    InGame_BG,
    Minigmae_BG
};

enum class SFXType
{
    HiddenObj_Playerin_Sound,               //은신 옵젝 들어갈 때 소리
    HiddenObj_Playerout_Sound,              //은신 옵젝 나올 때 소리
    HiddenObj_Nointeraction_Sound,          //은신 옵젝 은신 불가능할 때 상호작용 시 사운드
    FindObj_Interaction_1_Sound,            //수색 옵젝(조각보, 바구니) 수색 사운드
    FindObj_Interaction_2_Sound,            //수색 옵젝(박스) 수색 사운드
    FindObj_Acquiremap_Sound,               //수색 옵젝 지도 조각 확득 사운드
    FindObj_Acquireitem_Sound,              //수색 옵젝 재료 아이템 획득 사운드
    PitfallObj_Floor_Sound,                 //함정 오브젝트 (깨진 바닥) 사운드
    PitfallObj_Table_Sound,                 //함정 오브젝트 (낡은 책상) 사운드
    PitfallObj_SleepGhost_Sound,            //함정 오브젝트 (자는 애기 유령) 사운드
    PitfallObj_SleepGhost_Wakeup_Sound,     //함정 오브젝트 (자는 애기 유령) 깨어나는 사운드
    GoalObj_Sound                           //제사상에 음식 내려놓는 사운드
};

/// <summary>
/// 클라이언트 사운드 관리 매니저
/// 반드시 최소 2개의 AudioSourceComponent가 존재해야한다.
/// </summary>
class SoundManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    // singleton
    inline static SoundManager* s_instance = nullptr;

    SoundManager() = default;
    ~SoundManager() override = default;

    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    SoundManager(SoundManager&&) = delete;
    SoundManager& operator=(SoundManager&&) = delete;

    static SoundManager* Instance() { return s_instance; }

    // component process
    void OnInitialize() override;
    void OnStart() override;
    void OnDestory() override;

    void PlayBGM(BGMType type, bool restart = true);
    void StopBGM();
    void PauseBGM(bool paused);

    void PlaySFX(SFXType type); // 무조건 한 번

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    /// <summary>
    /// 그룹(채널 그룹) 볼륨 설정. (예: BGM, SFX 등)
    /// </summary>
    void SetGroupSoundVolume(const std::string& groupName, float volume)
    {
        AudioManager::Instance().GetSystem().SetChannelGroupVolume(groupName, volume);
    }

    /// <summary>
    /// 그룹(채널 그룹) 볼륨 조회. 그룹이 아직 없으면 마스터 볼륨을 반환할 수 있음.
    /// </summary>
    float GetGroupSoundVolume(const std::string& groupName) const
    {
        return AudioManager::Instance().GetSystem().GetChannelGroupVolume(groupName);
    }

    void SetMasterVolume(float volume)
    {
        AudioManager::Instance().GetSystem().SetMasterVolume(volume);
    }

    float GetMasterVolume() const
    {
        return AudioManager::Instance().GetSystem().GetMasterVolume();
    }

    AudioSourceComponent* bgmSource;                // NOTE : BGM은 1개만 실행된다.
    std::vector<AudioSourceComponent*> sfxSources;  // 8개 생성하고 번갈아가면서 사용하기

private:

    std::string ToString(BGMType type);
    std::string ToString(SFXType type);
};