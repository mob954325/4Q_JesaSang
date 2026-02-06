#include "AudioListenerSyncScript.h"

#include "Components/AudioListenerComponent.h"
#include "Components/Transform.h"
#include "Object/GameObject.h"
#include "Util/ComponentAutoRegister.h"
#include "Util/JsonHelper.h"

REGISTER_COMPONENT(AudioListenerSyncScript)

RTTR_REGISTRATION
{
    rttr::registration::class_<AudioListenerSyncScript>("AudioListenerSyncScript")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("EnableSync", &AudioListenerSyncScript::enableSync)
        .property("VelocityScale", &AudioListenerSyncScript::velocityScale);
}

void AudioListenerSyncScript::OnInitialize()
{
}

void AudioListenerSyncScript::OnUpdate(float delta)
{
    if (!enableSync || !m_Transform || !m_Listener || delta <= 0.0f)
    {
        if (auto owner = GetOwner())
        {
            if (!m_Transform) m_Transform = owner->GetTransform();
            if (!m_Listener) m_Listener = owner->GetComponent<AudioListenerComponent>();
            if (!m_HasInit && m_Transform)
            {
                m_PrevPos = m_Transform->GetWorldPosition();
                m_HasInit = true;
            }
        }
        return;
    }

    const Vector3 pos = m_Transform->GetWorldPosition();
    const Vector3 vel = (pos - m_PrevPos) / delta * velocityScale;
    m_PrevPos = pos;

    AudioTransform t{};
    t.position = { pos.x, pos.y, pos.z };
    t.velocity = { vel.x, vel.y, vel.z };
    t.forward = { 0.0f, 0.0f, 1.0f };
    t.up = { 0.0f, 1.0f, 0.0f };
    m_Listener->SetFallback(t);
}

nlohmann::json AudioListenerSyncScript::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void AudioListenerSyncScript::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}
