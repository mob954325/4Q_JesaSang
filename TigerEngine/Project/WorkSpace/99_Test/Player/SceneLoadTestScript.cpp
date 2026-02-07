#include "SceneLoadTestScript.h"
#include "Util/JsonHelper.h"
#include "EngineSystem/SceneSystem.h"
#include "Object/GameObject.h"
#include "../Base/System/InputSystem.h"
#include "Util/ComponentAutoRegister.h"
#include "../Base/pch.h"

REGISTER_COMPONENT(SceneLoadTestScript)

RTTR_REGISTRATION
{
    rttr::registration::class_<SceneLoadTestScript>("SceneLoadTestScript")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr);
}

void SceneLoadTestScript::OnInitialize()
{
}

void SceneLoadTestScript::OnEnable()
{
}

void SceneLoadTestScript::OnStart()
{
}

void SceneLoadTestScript::OnUpdate(float delta)
{
    if (Input::GetKeyDown(DirectX::Keyboard::Keys::Space))
    {
        SceneSystem::Instance().ChangeSceneByIndex(index);
        index++;
        if (index >= SceneSystem::Instance().GetSceneCount())
        {
            index = 0;
        }
    }
}

nlohmann::json SceneLoadTestScript::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void SceneLoadTestScript::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}