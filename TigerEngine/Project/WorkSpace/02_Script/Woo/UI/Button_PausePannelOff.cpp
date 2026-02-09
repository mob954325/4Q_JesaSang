#include "Button_PausePannelOff.h"
#include "../Engine/Util/JsonHelper.h"
#include "../Engine/Util/ComponentAutoRegister.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"

#include "../Engine/EngineSystem/SceneSystem.h"
#include "../Engine/Object/GameObject.h"
#include "../Engine/Components/UI/Image.h"

#include "../Manager/PauseManager.h"

REGISTER_COMPONENT(Button_PausePannelOff);

RTTR_REGISTRATION
{
    rttr::registration::class_<Button_PausePannelOff>("Button_PausePannelOff")
    .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("normalImagePath", &Button_PausePannelOff::normalImagePath)
        .property("pressedImagePath", &Button_PausePannelOff::GetPressImagePath, &Button_PausePannelOff::SetPressImagePath)
            (metadata(META_BROWSE, ""));
}

void Button_PausePannelOff::OnStart()
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
            image->ChangeData(pressedImagePath);
        });

    image->OnPressOut.AddListener(image, [this]()
        {
            image->ChangeData(normalImagePath);
            
            // 게임 이어하기
            auto* pm = SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName("PauseManager")->GetComponent<PauseManager>();
            if (!pm)
            {
                cout << "[Button] Missing Pause Manager!" << endl;
                return;
            }

            pm->Resume();
        });
}


nlohmann::json Button_PausePannelOff::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void Button_PausePannelOff::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

const std::string& Button_PausePannelOff::GetPressImagePath() const
{
    return pressedImagePath;
}

void Button_PausePannelOff::SetPressImagePath(const std::string& path)
{
    pressedImagePath = path;
}
