#include "SceneFlowManager.h"

#include "Util/JsonHelper.h"
#include "Util/ComponentAutoRegister.h"
#include "System/InputSystem.h"
#include <directxtk/Keyboard.h>
#include "EngineSystem/SceneSystem.h"

REGISTER_COMPONENT(SceneFlowManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<SceneFlowManager>("SceneFlowManager")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("StartScenePath", &SceneFlowManager::startScenePath)
        .property("IntroScenePath", &SceneFlowManager::introScenePath)
        .property("TutorialScenePath", &SceneFlowManager::tutorialScenePath)
        .property("MainScenePath", &SceneFlowManager::mainScenePath)
        .property("AllowNumberKeys", &SceneFlowManager::allowNumberKeys)
        .property("AllowStartAdvance", &SceneFlowManager::allowStartAdvance);
}

SceneFlowManager* SceneFlowManager::s_instance = nullptr;

void SceneFlowManager::OnInitialize()
{
    if (!s_instance)
    {
        s_instance = this;
    }
}

void SceneFlowManager::OnStart()
{
}

void SceneFlowManager::OnDestory()
{
    if (s_instance == this)
    {
        s_instance = nullptr;
    }
}

nlohmann::json SceneFlowManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void SceneFlowManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void SceneFlowManager::LoadScene(const std::string& path)
{
    if (path.empty())
    {
        return;
    }

    if (auto scene = SceneSystem::Instance().GetCurrentScene())
    {
        scene->LoadToJson(path);
    }
}

void SceneFlowManager::OnUpdate(float delta)
{
    (void)delta;

    if (allowNumberKeys)
    {
        if (InputSystem::Instance().GetKeyDown(DirectX::Keyboard::Keys::D1))
        {
            LoadScene(startScenePath);
        }
        else if (InputSystem::Instance().GetKeyDown(DirectX::Keyboard::Keys::D2))
        {
            LoadScene(introScenePath);
        }
        else if (InputSystem::Instance().GetKeyDown(DirectX::Keyboard::Keys::D3))
        {
            LoadScene(tutorialScenePath);
        }
        else if (InputSystem::Instance().GetKeyDown(DirectX::Keyboard::Keys::D4))
        {
            LoadScene(mainScenePath);
        }
    }

    if (allowStartAdvance)
    {
        if (InputSystem::Instance().GetKeyDown(DirectX::Keyboard::Keys::Space) ||
            InputSystem::Instance().GetKeyDown(DirectX::Keyboard::Keys::Enter))
        {
            LoadScene(introScenePath);
        }
    }
}
