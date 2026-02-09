#pragma once

#include <pch.h>
#include <System/Singleton.h>

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "..\\..\\Externals\\AudioModule_FMOD\\include\\AudioClip.h"
#include "..\\..\\Externals\\AudioModule_FMOD\\include\\AudioSystem.h"

struct AudioManifestEntry
{
    std::string id;                 // 사운드 구별용 ID
    std::string path;               // 사운드 파일 위치
    std::string group;              // 사운드 그룹?
    FMOD_MODE mode = FMOD_DEFAULT;  // FMode -> ??
    float defaultVolume = 1.0f;     // 해당 사운드가 가지고 있는 볼륨값
    bool loop = false;              // 루프 여부
};

class AudioManager : public Singleton<AudioManager>
{
public:
    AudioManager(token) {}
    ~AudioManager() = default;

    bool Initialize(const std::string& manifestPath = "");
    void Shutdown();
    void Update();

    // Initialize에서 호출하고 있음 ( 그럼 왜 public일까 이건? 편의성? )
    // <remarks>
    // 매개변수는 아마 고정 패스를 불러오기 위한 장치인 것으로 추정
    // 없으면 ResolveManifestPath 내에서 FindDirectory 유틸함수로 자동 등록하고 있음.
    // </remarks>
    bool LoadManifest(const std::string& manifestPath = "");

    // 
    const AudioManifestEntry* GetEntry(const std::string& id) const;

    std::shared_ptr<AudioClip> GetOrCreateClip(const std::string& id);

    void PreloadAll();

    std::vector<std::string> GetEntryIds() const;

    AudioSystem& GetSystem() { return m_system; }

private:
    std::filesystem::path ResolveManifestPath(const std::string& manifestPath) const;

    static std::string Trim(std::string value);
    static std::vector<std::string> SplitCsvLine(const std::string& line);
    static FMOD_MODE ParseMode(const std::string& text);

    std::unordered_map<std::string, AudioManifestEntry> m_entries;  // csv로 가져온 오디오 목록들
    std::unordered_map<std::string, std::shared_ptr<AudioClip>> m_clips;
    AudioSystem m_system;
    std::filesystem::path m_manifestPath;
    bool m_initialized = false;
};
