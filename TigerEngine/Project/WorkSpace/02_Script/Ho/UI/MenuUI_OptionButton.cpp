#include "MenuUI_OptionButton.h"
#include "../Engine/Util/JsonHelper.h"
#include "../Engine/Object/GameObject.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"
#include "../Engine/Util/ComponentAutoRegister.h"

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
                image->ChangeData(normalImagePath);
                // 시작 씬으로 전환
            }); // 누르는거 땠으면 이미지 바꾸기
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
