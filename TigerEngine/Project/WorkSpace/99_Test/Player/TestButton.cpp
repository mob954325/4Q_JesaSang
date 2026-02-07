#include "TestButton.h"
#include "Object/GameObject.h"
#include "Util/ComponentAutoRegister.h"
#include "Util/JsonHelper.h"

REGISTER_COMPONENT(TestButton);

RTTR_REGISTRATION
{
    rttr::registration::class_<TestButton>("TestButton")
    .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

void TestButton::ShowNormal() 
{
    imageNormal->SetActive(true);
    imageEnter->SetActive(false);
    imagePressed->SetActive(false);
}
void TestButton::ShowEnter() 
{
    imageNormal->SetActive(false);
    imageEnter->SetActive(true);
    imagePressed->SetActive(false);
}
void TestButton::ShowPressed() 
{
    imageNormal->SetActive(false);
    imageEnter->SetActive(false);
    imagePressed->SetActive(true);
}

void TestButton::OnInitialize()
{

}

void TestButton::OnEnable()
{
}

void TestButton::OnStart()
{
    // imageEnter = GetOwner()->AddComponent<Image>();
    // imageNormal = GetOwner()->AddComponent<Image>();
    // imagePressed = GetOwner()->AddComponent<Image>();
    // 
    // ShowNormal();
    // 
    // auto Bind = [this](Image* img)
    //     {
    //         img->OnEnter.AddListener(img, [this]()
    //             {
    //                 if (imagePressed->GetActiveSelf()) return;
    //                 ShowEnter();
    //             });
    // 
    //         img->OnExit.AddListener(img, [this]()
    //             {
    //                 if (imagePressed->GetActiveSelf()) return;
    //                 ShowNormal();
    //             });
    // 
    //         img->OnClick.AddListener(img, [this]()
    //             {
    //                 ShowPressed();
    //             });
    // 
    //         img->OnPressed.AddListener(img, [this]()
    //             {
    //                 if (!imagePressed->GetActiveSelf())
    //                     ShowPressed();
    //             });
    // 
    //         img->OnPressOut.AddListener(img, [this]()
    //             {
    //                 imagePressed->SetActive(false);
    //                 imageNormal->SetActive(false);
    //                 imageEnter->SetActive(true);
    //             });
    //     };
    // 
    // Bind(imageNormal);
    // Bind(imageEnter);
    // Bind(imagePressed);
}

void TestButton::OnDisable()
{
}

void TestButton::OnDestory()
{
}

void TestButton::OnUpdate(float delta)
{
}

nlohmann::json TestButton::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void TestButton::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}
