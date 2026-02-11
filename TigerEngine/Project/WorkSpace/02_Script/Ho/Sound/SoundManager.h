#pragma once
#include "../../../Base/System/Singleton.h"
#include <string>

#include "Manager/AudioManager.h"

/// <summary>
/// 클라이언트 사운드 관리 매니저
/// </summary>
class SoundManager : public Singleton<SoundManager>
{
public:
    SoundManager(token) {}

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
};
