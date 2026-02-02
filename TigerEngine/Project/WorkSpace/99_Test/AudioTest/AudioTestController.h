#pragma once

#include "Components/ScriptComponent.h"
#include "Components/AudioListenerComponent.h"
#include "Components/AudioSourceComponent.h"

#include <directxtk/Keyboard.h>

class Transform;

class AudioTestController : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    AudioTestController() { SetName("AudioTestController"); }
    ~AudioTestController() override = default;

    void OnInitialize() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    // Toggles
    bool enablePlayModeAuto = false;
    bool enableListenerSync = false;
    bool enableOrbit = false;
    bool enableKeyTrigger = false;

    // Play mode
    bool autoPlay = true;

    // Key trigger
    std::string clipId = "SFX_OneShot";
    int triggerKey = static_cast<int>(DirectX::Keyboard::Keys::Z);

    // Orbit
    float radius = 300.0f;
    float speed = 1.0f;
    float height = 0.0f;

    // Listener sync
    float listenerVelScale = 0.01f;

private:
    Transform* m_Transform = nullptr;
    AudioSourceComponent* m_Source = nullptr;
    AudioListenerComponent* m_Listener = nullptr;

    Vector3 m_Center{ 0.0f, 0.0f, 0.0f };
    Vector3 m_PrevPosSource{ 0.0f, 0.0f, 0.0f };
    Vector3 m_PrevPosListener{ 0.0f, 0.0f, 0.0f };
    float m_Time = 0.0f;

    bool m_WasPaused = false;
    bool m_HasStarted = false;
};
