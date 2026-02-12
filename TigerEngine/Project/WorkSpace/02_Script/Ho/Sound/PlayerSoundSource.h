#pragma once
#include "../../../Base/pch.h"
#include "../../../Engine/Components/ScriptComponent.h"
#include "../../../Engine/Components/AudioSourceComponent.h"

// NOTE TODO : 반드시 switch문의 ID값이 csv id값과 동일해야함.


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

enum class PlayerFootStepType
{
    Player_Walk_Sound1, // TODO 임시 지정한 단어임 나중에 수정해야함.
    Player_Walk_Sound2,
    Player_Walk_Sound3,
    Player_Walk_Sound4,
    Player_Walk_Sound5
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
    void StopSound();

    /// <summary>
    /// PlayerFootStepType 에서 랜덤으로 소리 재생하는 함수
    /// </summary>
    void PlayRandomFootStep();

private:
    AudioSourceComponent* sound = nullptr;
};
