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
    s_instance = this;
}

void GameManager::OnStart()
{
}

void GameManager::OnDestory()
{
    if (s_instance == this) s_instance = nullptr;
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
    // TODO 연출
    /*
        1. 마지막 음식이 제사상에 올라오는 순간 제사상에서 이팩트 발생 
        2. 화면이 전환되며 해피 엔딩 일러스트 등장
    */

    std::cout << "[GameManager] Game Success!" << endl;
}

void GameManager::GameOver()
{
    // TODO 연출
    /*
        1. 음산한 기운이 화면을 뒤덮음 
        2. 화면이 전환되며 배드 엔딩 일러스트 등장
    */

    std::cout << "[GameManager] Game Over..." << endl;
}
