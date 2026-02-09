#include "MainGameUIManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"

#include "EngineSystem/SceneSystem.h"
#include "System/InputSystem.h"
#include "System/TimeSystem.h"
#include "Object/GameObject.h"
#include "Components/RectTransform.h"
#include "Components/UI/Image.h"
#include "Components/UI/TextUI.h"


REGISTER_COMPONENT(MainGameUIManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<MainGameUIManager>("MainGameUIManager")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr)
    .property("lifeOnImagePath", &MainGameUIManager::lifeOnImagePath)  (metadata(META_BROWSE, ""))
    .property("lifeOffImagePath", &MainGameUIManager::lifeOffImagePath) (metadata(META_BROWSE, ""));
}

void MainGameUIManager::OnInitialize()
{
    s_instance = this;
}

void MainGameUIManager::OnStart()
{
    auto scene = SceneSystem::Instance().GetCurrentScene();
    if (!scene)
    {
        cout << "[MainGameUIManager] Current scene is null!" << endl;
        return;
    }

    auto SafeGetImage = [&](const std::string& objName) -> Image*
        {
            auto go = scene->GetGameObjectByName(objName);
            if (!go)
            {
                cout << "[MainGameUIManager] Missing GameObject: " << objName << endl;
                return nullptr;
            }

            auto img = go->GetComponent<Image>();
            if (!img)
            {
                cout << "[MainGameUIManager] Missing Image Component on: " << objName << endl;
                return nullptr;
            }

            return img;
        };

    // life image component
    life_1 = SafeGetImage("Image_Life1");
    life_2 = SafeGetImage("Image_Life2");
    life_3 = SafeGetImage("Image_Life3");
    life_4 = SafeGetImage("Image_Life4");
    life_5 = SafeGetImage("Image_Life5");

    if (!life_1 || !life_2 || !life_3 || !life_4 || !life_5)
    {
        cout << "[MainGameUIManager] Missing life UI objects!" << endl;
    }
}

void MainGameUIManager::OnDestory()
{
    if (s_instance == this) s_instance = nullptr;
}

nlohmann::json MainGameUIManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void MainGameUIManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void MainGameUIManager::UpdateLifeUI(int currentLife)
{
    if (!life_1 || !life_2 || !life_3 || !life_4 || !life_5)
    {
        cout << "[MainGameUIManager] UpdateLifeUI failed: life images not initialized." << endl;
        return;
    }

    if (currentLife < 0) currentLife = 0;
    if (currentLife > 5) currentLife = 5;

    Image* lifes[5] = { life_1, life_2, life_3, life_4, life_5 };

    for (int i = 0; i < 5; i++)
    {
        if (i < currentLife)
            lifes[i]->ChangeData(lifeOnImagePath);
        else
            lifes[i]->ChangeData(lifeOffImagePath);
    }
}
