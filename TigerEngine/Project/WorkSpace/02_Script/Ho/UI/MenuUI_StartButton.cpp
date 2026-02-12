#include "MenuUI_StartButton.h"
#include "../Engine/Util/JsonHelper.h"
#include "../Engine/Object/GameObject.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"
#include "../Engine/Util/ComponentAutoRegister.h"
#include "../Engine/EngineSystem/SceneSystem.h"
#include "../Engine/Components/AudioSourceComponent.h"

namespace
{
    constexpr const char* kStartSceneButtonClickClipId = "Intro_Button_Sound";
}

REGISTER_COMPONENT(MenuUI_StartButton);

RTTR_REGISTRATION
{
    rttr::registration::class_<MenuUI_StartButton>("MenuUI_StartButton")
    .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("pressedImagePath", &MenuUI_StartButton::GetPressImagePath, &MenuUI_StartButton::SetPressImagePath)
            (metadata(META_BROWSE, "")) // 탐색
        .property("normalImagePath", &MenuUI_StartButton::normalImagePath)
        .property("targetScenePath", &MenuUI_StartButton::targetScenePath)
            (metadata(META_BROWSE, ""));
}

void MenuUI_StartButton::OnInitialize()
{
}

void MenuUI_StartButton::OnEnable()
{
}

void MenuUI_StartButton::OnStart()
{
    // 등록 후 시작 전에 해당 게임 오브젝트에 컴포넌트가 있는지 확인
    image = GetOwner()->GetComponent<Image>();
    EnsureClickAudioSource();


    if (image)
    {
        normalImagePath = image->GetPath();

        image->OnPressed.AddListener(image, [this]() 
            { 
                image->ChangeData(pressedImagePath); 
                
            }); // 누르면 이미지 바꾸기

        image->OnPressOut.AddListener(image, [this]() 
            { 
                image->ChangeData(normalImagePath); 
                PlayClickSound();

                // 시작 씬으로 전환 
                // NOTE : 지정하는게 browse면 상관없을 거임
                auto s = SceneSystem::Instance().GetCurrentScene();
                s->LoadToJson(targetScenePath); // 해당함수는 경로를 저장한다 -> 잘못 패스를 저장하고 PlayMode에서 정지하면 씬 정보가 날라간다

            }); // 누르는거 땠으면 이미지 바꾸기
    }
}

void MenuUI_StartButton::OnDisable()
{
}

void MenuUI_StartButton::OnDestory()
{
}

void MenuUI_StartButton::OnUpdate(float delta)
{
    if (image == nullptr) return;

}

nlohmann::json MenuUI_StartButton::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void MenuUI_StartButton::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

const std::string& MenuUI_StartButton::GetPressImagePath() const
{
    return pressedImagePath;
}

void MenuUI_StartButton::SetPressImagePath(const std::string& path)
{
    pressedImagePath = path;
}

void MenuUI_StartButton::EnsureClickAudioSource()
{
    auto* owner = GetOwner();
    if (!owner)
    {
        return;
    }

    m_ClickAudioSource = owner->GetComponent<AudioSourceComponent>();
    if (!m_ClickAudioSource)
    {
        m_ClickAudioSource = owner->AddComponent<AudioSourceComponent>();
    }
}

void MenuUI_StartButton::PlayClickSound()
{
    if (!m_ClickAudioSource)
    {
        return;
    }

    m_ClickAudioSource->SetChannelGroup("SFX");
    m_ClickAudioSource->SetLoop(false);
    m_ClickAudioSource->SetClipId(kStartSceneButtonClickClipId);
    m_ClickAudioSource->Play(true);
}
