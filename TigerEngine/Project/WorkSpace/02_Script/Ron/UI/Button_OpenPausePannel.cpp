#include "Button_OpenPausePannel.h"

#include "../Engine/Util/JsonHelper.h"
#include "../Engine/Util/ComponentAutoRegister.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"

#include "../Engine/EngineSystem/SceneSystem.h"
#include "../Engine/Object/GameObject.h"
#include "../Engine/Components/UI/Image.h"
#include "System/TimeSystem.h"
#include "../../Ron/UI/SettingsUIController.h"

REGISTER_COMPONENT(Button_OpenPausePannel);

RTTR_REGISTRATION
{
    rttr::registration::class_<Button_OpenPausePannel>("Button_OpenPausePannel")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("normalImagePath", &Button_OpenPausePannel::normalImagePath)
        .property("pressedImagePath", &Button_OpenPausePannel::GetPressImagePath, &Button_OpenPausePannel::SetPressImagePath)
            (metadata(META_BROWSE, ""));
}

void Button_OpenPausePannel::OnStart()
{
    image = GetOwner()->GetComponent<Image>();
    if (!image) {
        cout << "[Button] Missing Image Compoennt!" << endl;
        return;
    }

    // path 저장
    normalImagePath = image->GetPath();

    // button event
    image->OnPressed.AddListener(image, [this]()
        {
            if (!pressedImagePath.empty())
            {
                image->ChangeData(pressedImagePath);
            }
        });

    image->OnPressOut.AddListener(image, [this]()
        {
            if (!normalImagePath.empty())
            {
                image->ChangeData(normalImagePath);
            }
            
            auto scene = SceneSystem::Instance().GetCurrentScene();
            if (!scene) return;

            if (auto settingsObj = scene->GetGameObjectByName("UI_Settings"))
            {
                if (auto settings = settingsObj->GetComponent<SettingsUIController>())
                {
                    settings->SetOpen(false);
                }
            }

            if (auto pausePannel = scene->GetGameObjectByName("PausePannel"))
            {
                pausePannel->SetActive(true);
            }

            // 게임 시간 정지
            GameTimer::Instance().SetTimeScale(0.0f);
        });
}

nlohmann::json Button_OpenPausePannel::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void Button_OpenPausePannel::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

const std::string& Button_OpenPausePannel::GetPressImagePath() const
{
    return pressedImagePath;
}

void Button_OpenPausePannel::SetPressImagePath(const std::string& path)
{
    pressedImagePath = path;
}
