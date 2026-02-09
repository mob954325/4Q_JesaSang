#include "MainGameUIManager.h"
#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"

REGISTER_COMPONENT(MainGameUIManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<MainGameUIManager>("MainGameUIManager")
    .constructor<>()
    (rttr::policy::ctor::as_std_shared_ptr);
}

void MainGameUIManager::OnInitialize()
{
}

void MainGameUIManager::OnStart()
{
}

nlohmann::json MainGameUIManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void MainGameUIManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}
