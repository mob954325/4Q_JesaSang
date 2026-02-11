#include "CookingZone.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "EngineSystem/SceneSystem.h"
#include "Object/GameObject.h"
#include "Components/UI/Image.h"

REGISTER_COMPONENT(CookingZone)

RTTR_REGISTRATION
{
    rttr::registration::class_<CookingZone>("CookingZone")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void CookingZone::OnInitialize()
{
    // 중복 생성 방지
    if (s_instance != nullptr && s_instance != this)
    {
        assert(false && "Duplicate GameManager instance detected.");
        return;
    }

    s_instance = this;
}

void CookingZone::OnStart()
{
    const auto& sceneSystem = SceneSystem::Instance().GetCurrentScene();

    // cooking zone ui find
    image_sensorOn = sceneSystem->GetGameObjectByName("Image_SensorOn_Cook")->GetComponent<Image>();
    image_interactionOn = sceneSystem->GetGameObjectByName("Image_InteractionOn_Cook")->GetComponent<Image>();
    image_interactionGauge = sceneSystem->GetGameObjectByName("Image_InteractionGauge_Cook")->GetComponent<Image>();

    if (!image_sensorOn || !image_interactionOn || !image_interactionGauge)
    {
        cout << "[CookingZone] Missing ui!" << endl;
        return;
    }
}

void CookingZone::OnDestory()
{
    if (s_instance == this) s_instance = nullptr;
}

nlohmann::json CookingZone::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void CookingZone::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void CookingZone::UISensorOnOff(bool flag)
{
    if (!image_sensorOn) return;
    image_sensorOn->SetActive(flag);
}

void CookingZone::UIInteractionOnOff(bool flag)
{
    if (!image_interactionOn) return;
    image_interactionOn->SetActive(flag);
    image_interactionGauge->SetActive(flag);
}

void CookingZone::UIGaugeUpate(float progress)
{
    if (!image_interactionGauge) return;
    image_interactionGauge->SetFillAmount(progress);
}