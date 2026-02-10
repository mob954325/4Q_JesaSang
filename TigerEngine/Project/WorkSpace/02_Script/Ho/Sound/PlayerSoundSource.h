#pragma once
#include "../../../Base/pch.h"
#include "../../../Engine/Components/ScriptComponent.h"
#include "../../../Engine/Components/AudioSourceComponent.h"

enum class PlayerSoundType
{
    Sit,            // 플레이어 앉기
    Walk,           // 플레이어 걷기
    Run,            // 플레이어 뛰기
    Hit,            // 플레이어 피격
    Confused,       // 플레이어 혼란
    HpDown,         // 플레이어 체력 감소
    Typing          // 타이핑
};

class PlayerSoundSource : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    void OnStart() override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    /// <summary>
    /// 외부에서 호출할 사운드 출력함수
    /// </summary>
    /// <param name="type">플레이어 사운드 타입</param>
    void PlaySound(PlayerSoundType type, bool loop = false);

private:
    AudioSourceComponent* sound = nullptr;
};
