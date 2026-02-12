#pragma once
#include "System/InputSystem.h"
#include "Components/ScriptComponent.h"
#include "Weapon.h"
#include "../Engine/Components/AudioSourceComponent.h"
#include "../../02_Script/Ho/Sound/PlayerSoundSource.h"
#include "../../02_Script/Ho/Sound/SoundManager.h"
#include "../Engine/Components/UI/TextUI.h"

/// <summary>
/// 클라이언트 컴포넌트 테스트용 클래스.
/// </summary>
class Player1 : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    void OnInitialize() override;
    void OnEnable() override;
    void OnStart() override;
    void OnDisable() override;
    void OnDestory() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    float bgmValue = 1.0f;
    float sfxValue = 1.0f;
    float masterValue = 1.0f;

private:
    PlayerSoundSource* pss;
    AudioListenerComponent* alc;
    Vector3 prevPos{};
    bool hasPrev = false;    

    void UpdateAudioTransform(float dt);


    SoundManager* soundManager;
    TextUI* text{};
};

