#include "AudioListenerComponent.h"

#include "..\\Manager\\AudioManager.h"
#include "..\\Util\\JsonHelper.h"
#include "../../Engine/Object/GameObject.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<AudioListenerComponent>("AudioListenerComponent")
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr);
}


void AudioListenerComponent::OnInitialize()
{
    if (!m_System)
    {
        Init(&AudioManager::Instance().GetSystem());
    }
}

void AudioListenerComponent::OnStart()
{

}

void AudioListenerComponent::OnUpdate(float delta)
{
    (void)delta;
    if (!m_System) return;

    if (!m_System || !m_System->GetSystem())
    {
        return;
    }

    // 3d apply
    const AudioTransform t = ResolveAudioTransform(m_Fallback, m_Bind);
    m_Listener.Set(t.position, t.velocity, t.forward, t.up);
    m_Listener.Apply(m_System->GetSystem());
}

void AudioListenerComponent::OnDestory()
{
    m_System = nullptr;
}

nlohmann::json AudioListenerComponent::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void AudioListenerComponent::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void AudioListenerComponent::Init(AudioSystem* system)
{
    m_System = system;
}

void AudioListenerComponent::BindTransform(const AudioTransformRef& ref)
{
    m_Bind = ref;
}

void AudioListenerComponent::SetFallback(const AudioTransform& transform)
{
    m_Fallback = transform;
}