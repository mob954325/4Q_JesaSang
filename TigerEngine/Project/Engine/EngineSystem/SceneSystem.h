#pragma once
#include "pch.h"
#include "../Scene/Scene.h"
#include <map>
#include <vector>
#include "System/Singleton.h"

class SceneSystem : public Singleton<SceneSystem>
{
public:
    SceneSystem(token) {};
    ~SceneSystem() = default;

    void BeforUpdate();
    void UpdateScene(float deltaTime);
    void FixedUpdateScene(float deltaTime);
    void LateUpdateScene(float deltaTime);
    void CheckSceneShange();

    std::shared_ptr<Scene> GetSceneByIndex(int index);
    void AddScene();
    std::shared_ptr<Scene> GetCurrentScene();
    std::shared_ptr<Scene> SetCurrentSceneByIndex(int i = 0);

    bool isSceneChanging = false; // 현재 씬 교체 중인가? -> 사용안함

private:
    std::map<int, std::shared_ptr<Scene>> scenes;
    std::shared_ptr<Scene> currentScene{};
};

namespace SceneUtil
{
    inline GameObject* GetObjectByName(std::string name) { return SceneSystem::Instance().GetCurrentScene()->GetGameObjectByName(name); }
    inline GameObject* CreateGameObject(std::string name = "nonamed") { return SceneSystem::Instance().GetCurrentScene()->AddGameObjectByName(name); }
    inline std::vector<GameObject*> GetObjectsByName(const std::string& name) { return SceneSystem::Instance().GetCurrentScene()->GetGameObjectsByName(name); }
}