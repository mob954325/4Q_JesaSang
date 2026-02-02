#pragma once

#include "Components/ScriptComponent.h"

class AudioListenerComponent;
class Transform;

class AudioListenerSyncScript : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    AudioListenerSyncScript() { SetName("AudioListenerSyncScript"); }
    ~AudioListenerSyncScript() override = default;

    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    bool enableSync = true;
    float velocityScale = 0.01f;

private:
    Transform* m_Transform = nullptr;
    AudioListenerComponent* m_Listener = nullptr;
    Vector3 m_PrevPos{ 0.0f, 0.0f, 0.0f };
};
