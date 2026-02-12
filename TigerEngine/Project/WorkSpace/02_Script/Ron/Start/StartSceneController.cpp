#include "StartSceneController.h"

#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "System/InputSystem.h"
#include "EngineSystem/SceneSystem.h"
#include "Manager/UIManager.h"
#include "Object/GameObject.h"
#include "Components/AudioSourceComponent.h"
#include "Components/UI/Image.h"
#include "Components/RectTransform.h"

REGISTER_COMPONENT(StartSceneController)

namespace
{
    constexpr const char* kTitleBgmClipId = "Intro_Title_BG";
}

RTTR_REGISTRATION
{
    rttr::registration::class_<StartSceneController>("StartSceneController")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("ImageObjectName", &StartSceneController::imageObjectName)
        .property("NextScenePath", &StartSceneController::nextScenePath);
}

void StartSceneController::OnInitialize()
{
}

void StartSceneController::OnStart()
{
    auto scene = SceneSystem::Instance().GetCurrentScene();
    if (!scene)
    {
        return;
    }

    if (auto obj = scene->GetGameObjectByName(imageObjectName))
    {
        m_Image = obj->GetComponent<Image>();
        m_ImageRect = obj->GetComponent<RectTransform>();
    }

    EnsureAudioSource();
    PlayTitleBgm();
    ApplyLayout();
}

void StartSceneController::OnUpdate(float delta)
{
    (void)delta;

    if (!m_LayoutApplied)
    {
        ApplyLayout();
    }

    if (m_AudioSource && !m_AudioSource->IsPlaying())
    {
        PlayTitleBgm();
    }

    const bool spaceDown = Input::GetKey(DirectX::Keyboard::Keys::Space);
    const bool enterDown = Input::GetKey(DirectX::Keyboard::Keys::Enter);

    if (IsAdvancePressed(spaceDown, m_PrevSpace) || IsAdvancePressed(enterDown, m_PrevEnter))
    {
        LoadNextScene();
    }
}

nlohmann::json StartSceneController::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void StartSceneController::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void StartSceneController::ApplyLayout()
{
    Vector2 screen = UIManager::Instance().GetSize();
    if (screen.x <= 0.0f || screen.y <= 0.0f)
    {
        return;
    }

    if (m_ImageRect)
    {
        m_ImageRect->SetPivot({ 0.0f, 0.0f });
        m_ImageRect->SetPos({ 0.0f, 0.0f, 0.0f });
        m_ImageRect->SetSize(screen);
    }

    if (m_Image)
    {
        m_Image->ChangeData("..\\Assets\\Resource\\CutScene\\Title_fix.png");
    }

    m_LayoutApplied = true;
}

void StartSceneController::EnsureAudioSource()
{
    auto* owner = GetOwner();
    if (!owner)
    {
        return;
    }

    m_AudioSource = owner->GetComponent<AudioSourceComponent>();
    if (!m_AudioSource)
    {
        m_AudioSource = owner->AddComponent<AudioSourceComponent>();
    }
}

void StartSceneController::PlayTitleBgm()
{
    if (!m_AudioSource)
    {
        return;
    }

    m_AudioSource->SetChannelGroup("BGM");
    m_AudioSource->SetLoop(true);
    m_AudioSource->SetClipId(kTitleBgmClipId);
    m_AudioSource->Play(true);
}

void StartSceneController::LoadNextScene()
{
    if (nextScenePath.empty())
    {
        return;
    }

    if (auto scene = SceneSystem::Instance().GetCurrentScene())
    {
        scene->LoadToJson(nextScenePath);
    }
}

bool StartSceneController::IsAdvancePressed(bool down, bool& prevDown)
{
    const bool pressed = down && !prevDown;
    prevDown = down;
    return pressed;
}
