#pragma once
#include "../../../Engine/Components/ScriptComponent.h"
#include "../../../Engine/Components/UI/Image.h"
#include "EdgeToCenterSampler.h"

/// <summary>
/// 승리 시 활성화 되는 오브젝트에 붙을 컴포넌트
/// </summary>
class WinPanel : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;
    void UpdateWorldSetting(float dt); // 월드 세팅 조절해서 효과 만든

    void Play(); // notified = true

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    bool notified = false;  // 외부 호출을 했는가? -> Manager에서 요청
    bool isPlayed = false;  // 한 번 플레이했는가? -> 중복 플레이 방지

    float firstWaitTime = 0.0f; 
    float firstWaitMaxTime = 1.5f; 

    float timer = 0.0f;     // 이미지 시퀀스 시간
    float maxTimer = 0.1f;  // 이미지 시퀀스 딜레이 시간
    int index = 0;          // 이미지 시퀀스 인덱스

private:
    // World postprocess (승천 연출용, 0~4초 램프)
    bool worldSettingInited = false;
    float worldSettingTimer = 0.0f;
    float baseExposure = 0.0f;
    BOOL baseUseBloom = false;
    float baseBloomIntensity = 0.0f;
    float baseBloomThreshold = 1.0f;
    float baseBloomScatter = 0.5f;
    float baseBloomClamp = 0.0f;
    Vector3 baseBloomTint = { 1.0f, 1.0f, 1.0f };

    Image* cutImg = nullptr;
    std::string cutPath = "..\\Assets\\Resource\\CutScene\\intro_01_1.png";
    // particleUI -> // img가 있는 게임오브젝트를 복제 생성해서 여기저기 뿌린다. -> 오브젝트 제거 시에 모두 제거

    // 이미지 패스 모음
    std::vector<std::string> effectsPath{};
};
