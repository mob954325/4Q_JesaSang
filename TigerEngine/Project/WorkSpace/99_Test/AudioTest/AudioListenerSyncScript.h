#pragma once

#include "Object/Component.h"

class AudioListenerComponent;
class Transform;

class AudioListenerSyncScript : public Component
{
    RTTR_ENABLE(Component)
public:
    AudioListenerSyncScript() { SetName("AudioListenerSyncScript"); }
    ~AudioListenerSyncScript() override = default;

    void OnInitialize() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    bool enableSync = true;
    float velocityScale = 0.01f;

private:
    Transform* m_Transform = nullptr;
    AudioListenerComponent* m_Listener = nullptr;
    Vector3 m_PrevPos{ 0.0f, 0.0f, 0.0f };
    bool m_HasInit = false;
};
