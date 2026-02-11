#pragma once

#include "pch.h"
#include "../Object/Component.h"
#include "..\\..\\Externals\\AudioModule_FMOD\\include\\AudioListener.h"
#include "..\\..\\Externals\\AudioModule_FMOD\\include\\AudioTransform.h"

class AudioSystem;

class AudioListenerComponent : public Component
{
    RTTR_ENABLE(Component)
public:

    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;
    void OnDestory() override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    void Init(AudioSystem* system);
    void BindTransform(const AudioTransformRef& ref);
    void SetFallback(const AudioTransform& transform);

private:
    AudioSystem* m_System = nullptr;
    AudioListener m_Listener{};
    AudioTransform m_Fallback{};
    AudioTransformRef m_Bind{};
};
