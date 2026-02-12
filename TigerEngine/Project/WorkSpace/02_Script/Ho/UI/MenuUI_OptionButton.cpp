#include "MenuUI_OptionButton.h"
#include "../Engine/Util/JsonHelper.h"
#include "../Engine/Object/GameObject.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"
#include "../Engine/Util/ComponentAutoRegister.h"
#include "../../Ron/UI/SettingsUIController.h"
#include "../Engine/EngineSystem/SceneSystem.h"

REGISTER_COMPONENT(MenuUI_OptionButton);

RTTR_REGISTRATION
{
    rttr::registration::class_<MenuUI_OptionButton>("MenuUI_OptionButton")
    .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("pressedImagePath", &MenuUI_OptionButton::GetPressImagePath, &MenuUI_OptionButton::SetPressImagePath)
            (metadata(META_BROWSE, "")) // 탐색
        .property("normalImagePath", &MenuUI_OptionButton::normalImagePath)
        .property("targetScenePath", &MenuUI_OptionButton::targetScenePath)
            (metadata(META_BROWSE, ""));
}

void MenuUI_OptionButton::OnInitialize()
{
}

void MenuUI_OptionButton::OnEnable()
{
}

void MenuUI_OptionButton::OnStart()
{
    // 등록 후 시작 전에 해당 게임 오브젝트에 컴포넌트가 있는지 확인
    image = GetOwner()->GetComponent<Image>();


    if (image)
    {
        normalImagePath = image->GetPath();

        image->OnPressed.AddListener(image, [this]()
            {
                image->ChangeData(pressedImagePath);
            }); // 누르면 이미지 바꾸기

        image->OnPressOut.AddListener(image, [this]()
            {
                {
                    image->ChangeData(normalImagePath);
                    SettingsUIController* settings = SettingsUIController::Instance();
                    if (settings != nullptr)
                        settings->ToggleRoot();
                    return;
                }

                if (auto scene = SceneSystem::Instance().GetCurrentScene())
                {
                    if (auto obj = scene->GetGameObjectByName("UI_Settings"))
                    {
                        if (auto ctrl = obj->GetComponent<SettingsUIController>())
                        {
                            ctrl->ToggleRoot();
                            return;
                        }
                        // Fallback: toggle panels directly if controller is missing
                        auto soundPanel = scene->GetGameObjectByName("UI_Settings_SoundPanel");
                        auto creditPanel = scene->GetGameObjectByName("UI_Settings_CreditPanel");
                        auto soundButton = scene->GetGameObjectByName("UI_Settings_SoundButton");
                        auto creditButton = scene->GetGameObjectByName("UI_Settings_CreditButton");
                        const bool open = soundPanel ? !soundPanel->GetActiveSelf() : true;
                        if (soundPanel) soundPanel->SetActive(open);
                        if (creditPanel) creditPanel->SetActive(false);
                        if (soundButton) soundButton->SetActive(open);
                        if (creditButton) creditButton->SetActive(open);
                    }
                }
            });
    }
}

void MenuUI_OptionButton::OnDisable()
{
}

void MenuUI_OptionButton::OnDestory()
{
}

void MenuUI_OptionButton::OnUpdate(float delta)
{
    if (image == nullptr) return;

}

nlohmann::json MenuUI_OptionButton::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void MenuUI_OptionButton::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

const std::string& MenuUI_OptionButton::GetPressImagePath() const
{
    return pressedImagePath;
}

void MenuUI_OptionButton::SetPressImagePath(const std::string& path)
{
    pressedImagePath = path;
}
