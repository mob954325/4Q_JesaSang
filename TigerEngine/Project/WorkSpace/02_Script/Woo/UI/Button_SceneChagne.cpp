#include "Button_SceneChagne.h"
#include "../Engine/Util/JsonHelper.h"
#include "../Engine/Util/ComponentAutoRegister.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"

#include "../Engine/EngineSystem/SceneSystem.h"
#include "../Engine/Object/GameObject.h"
#include "../Engine/Components/UI/Image.h"

REGISTER_COMPONENT(Button_SceneChagne);

RTTR_REGISTRATION
{
    rttr::registration::class_<Button_SceneChagne>("Button_SceneChagne")
    .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("normalImagePath", &Button_SceneChagne::normalImagePath)
        .property("pressedImagePath", &Button_SceneChagne::GetPressImagePath, &Button_SceneChagne::SetPressImagePath)
            (metadata(META_BROWSE, ""))
        .property("targetScenePath", &Button_SceneChagne::targetScenePath)
            (metadata(META_BROWSE, ""));
}

void Button_SceneChagne::OnStart()
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

            // scene change
            auto s = SceneSystem::Instance().GetCurrentScene();
            s->LoadToJson(targetScenePath);
        });
}


nlohmann::json Button_SceneChagne::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void Button_SceneChagne::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

const std::string& Button_SceneChagne::GetPressImagePath() const
{
    return pressedImagePath;
}

void Button_SceneChagne::SetPressImagePath(const std::string& path)
{
    pressedImagePath = path;
}
