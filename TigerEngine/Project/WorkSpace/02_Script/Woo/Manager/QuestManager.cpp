#include "QuestManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "Object/GameObject.h"
#include "EngineSystem/SceneSystem.h"



REGISTER_COMPONENT(QuestManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<QuestManager>("QuestManager")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void QuestManager::OnInitialize()
{
    s_instance = this;
}

void QuestManager::OnStart()
{
}

void QuestManager::OnDestory()
{
    if (s_instance == this) s_instance = nullptr;
}

nlohmann::json QuestManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void QuestManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}