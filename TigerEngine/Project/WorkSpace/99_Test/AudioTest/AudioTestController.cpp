#include "AudioTestController.h"

#include <cmath>

#include "Object/GameObject.h"
#include "Components/Transform.h"
#include "EngineSystem/PlayModeSystem.h"
#include "System/InputSystem.h"
#include "Manager/AudioManager.h"
#include "Util/ComponentAutoRegister.h"

REGISTER_COMPONENT(AudioTestController);

RTTR_REGISTRATION
{
    rttr::registration::class_<AudioTestController>("AudioTestController")
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr)
        .property("EnablePlayModeAuto", &AudioTestController::enablePlayModeAuto)
        .property("EnableListenerSync", &AudioTestController::enableListenerSync)
        .property("EnableOrbit", &AudioTestController::enableOrbit)
        .property("EnableKeyTrigger", &AudioTestController::enableKeyTrigger)
        .property("AutoPlay", &AudioTestController::autoPlay)
        .property("ClipId", &AudioTestController::clipId)
        .property("TriggerKey", &AudioTestController::triggerKey)
        .property("Radius", &AudioTestController::radius)
        .property("Speed", &AudioTestController::speed)
        .property("Height", &AudioTestController::height)
        .property("ListenerVelScale", &AudioTestController::listenerVelScale);
}

void AudioTestController::OnInitialize()
{
    if (auto owner = GetOwner())
    {
        m_Transform = owner->GetTransform();
        m_Source = owner->GetComponent<AudioSourceComponent>();
        m_Listener = owner->GetComponent<AudioListenerComponent>();

        if (m_Transform)
        {
            const Vector3 pos = m_Transform->GetWorldPosition();
            m_Center = pos;
            m_PrevPosSource = pos;
            m_PrevPosListener = pos;
        }
    }
}

void AudioTestController::OnUpdate(float delta)
{
    if (!m_Transform || (!m_Source && !m_Listener))
    {
        if (auto owner = GetOwner())
        {
            if (!m_Transform) m_Transform = owner->GetTransform();
            if (!m_Source) m_Source = owner->GetComponent<AudioSourceComponent>();
            if (!m_Listener) m_Listener = owner->GetComponent<AudioListenerComponent>();
        }
    }

    if (enablePlayModeAuto && m_Source && autoPlay)
    {
        PlayModeState curr = PlayModeSystem::Instance().GetPlayMode();

        if (curr == PlayModeState::Playing)
        {
            if (!m_HasStarted)
            {
                m_Source->Play(true);
                m_HasStarted = true;
                m_WasPaused = false;
            }
            else if (m_WasPaused)
            {
                m_Source->Pause(false);
                m_WasPaused = false;
            }
        }
        else if (curr == PlayModeState::Paused)
        {
            if (!m_WasPaused)
            {
                m_Source->Pause(true);
                m_WasPaused = true;
            }
        }
        else if (curr == PlayModeState::Stopped)
        {
            if (m_HasStarted || m_Source->IsPlaying())
            {
                m_Source->Stop();
            }
            m_HasStarted = false;
            m_WasPaused = false;
        }
    }

    if (enableKeyTrigger && m_Source)
    {
        if (Input::GetKeyDown(static_cast<DirectX::Keyboard::Keys>(triggerKey)))
        {
            auto clip = AudioManager::Instance().GetOrCreateClip(clipId);
            if (clip)
            {
                m_Source->SetClip(std::move(clip));
                m_Source->SetLoop(false);
                m_Source->PlayOneShot();
            }
        }
    }

    if (enableOrbit && m_Transform && delta > 0.0f)
    {
        m_Time += delta;
        float phase = m_Time * speed;

        constexpr float kSegmentSeconds = 6.0f;
        const float segment = std::fmod(m_Time, kSegmentSeconds * 4.0f);
        const int mode = static_cast<int>(segment / kSegmentSeconds);

        Vector3 pos = m_Center;
        switch (mode)
        {
        case 0: // Left/Right
            pos.x = m_Center.x + std::sin(phase) * radius;
            pos.y = m_Center.y + height;
            break;
        case 1: // Up/Down
            pos.y = m_Center.y + std::sin(phase) * height;
            break;
        case 2: // Forward/Back
            pos.z = m_Center.z + std::sin(phase) * radius;
            pos.y = m_Center.y + height;
            break;
        default: // Diagonal sweep
            pos.x = m_Center.x + std::sin(phase) * radius;
            pos.y = m_Center.y + std::sin(phase) * height;
            pos.z = m_Center.z + std::sin(phase) * radius;
            break;
        }

        m_Transform->SetPosition(pos);

        // Convert cm/s to m/s for FMOD doppler.
        Vector3 vel = (pos - m_PrevPosSource) / delta * 0.01f;
        m_PrevPosSource = pos;

        if (m_Source)
        {
            AudioTransform t{};
            t.position = { pos.x, pos.y, pos.z };
            t.velocity = { vel.x, vel.y, vel.z };
            t.forward = { 0.0f, 0.0f, 1.0f };
            t.up = { 0.0f, 1.0f, 0.0f };
            m_Source->SetFallback(t);
        }
    }

    if (enableListenerSync && m_Transform && m_Listener && delta > 0.0f)
    {
        const Vector3 pos = m_Transform->GetWorldPosition();
        Vector3 vel = (pos - m_PrevPosListener) / delta * listenerVelScale;
        m_PrevPosListener = pos;

        AudioTransform t{};
        t.position = { pos.x, pos.y, pos.z };
        t.velocity = { vel.x, vel.y, vel.z };
        t.forward = { 0.0f, 0.0f, 1.0f };
        t.up = { 0.0f, 1.0f, 0.0f };
        m_Listener->SetFallback(t);
    }
}

nlohmann::json AudioTestController::Serialize()
{
    nlohmann::json data;
    rttr::type t = rttr::type::get(*this);
    data["type"] = t.get_name().to_string();
    data["properties"] = nlohmann::json::object();

    data["properties"]["EnablePlayModeAuto"] = enablePlayModeAuto;
    data["properties"]["EnableListenerSync"] = enableListenerSync;
    data["properties"]["EnableOrbit"] = enableOrbit;
    data["properties"]["EnableKeyTrigger"] = enableKeyTrigger;
    data["properties"]["AutoPlay"] = autoPlay;
    data["properties"]["ClipId"] = clipId;
    data["properties"]["TriggerKey"] = triggerKey;
    data["properties"]["Radius"] = radius;
    data["properties"]["Speed"] = speed;
    data["properties"]["Height"] = height;
    data["properties"]["ListenerVelScale"] = listenerVelScale;

    return data;
}

void AudioTestController::Deserialize(nlohmann::json data)
{
    if (!data.contains("properties"))
    {
        return;
    }

    const auto& props = data["properties"];
    if (props.contains("EnablePlayModeAuto")) enablePlayModeAuto = props["EnablePlayModeAuto"].get<bool>();
    if (props.contains("EnableListenerSync")) enableListenerSync = props["EnableListenerSync"].get<bool>();
    if (props.contains("EnableOrbit")) enableOrbit = props["EnableOrbit"].get<bool>();
    if (props.contains("EnableKeyTrigger")) enableKeyTrigger = props["EnableKeyTrigger"].get<bool>();
    if (props.contains("AutoPlay")) autoPlay = props["AutoPlay"].get<bool>();
    if (props.contains("ClipId")) clipId = props["ClipId"].get<std::string>();
    if (props.contains("TriggerKey")) triggerKey = props["TriggerKey"].get<int>();
    if (props.contains("Radius")) radius = props["Radius"].get<float>();
    if (props.contains("Speed")) speed = props["Speed"].get<float>();
    if (props.contains("Height")) height = props["Height"].get<float>();
    if (props.contains("ListenerVelScale")) listenerVelScale = props["ListenerVelScale"].get<float>();
}
