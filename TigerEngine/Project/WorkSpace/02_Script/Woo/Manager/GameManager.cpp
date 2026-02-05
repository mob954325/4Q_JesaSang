#include "GameManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"
#include "EngineSystem/SceneSystem.h"


REGISTER_COMPONENT(GameManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<GameManager>("GameManager")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void GameManager::OnInitialize()
{
}

void GameManager::OnStart()
{
}

void GameManager::OnDestory()
{
}

nlohmann::json GameManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void GameManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void GameManager::GameSuccess()
{
    std::cout << "[GameManager] Game Success!" << endl;
}

void GameManager::GameOver()
{
    std::cout << "[GameManager] Game Over..." << endl;
}
