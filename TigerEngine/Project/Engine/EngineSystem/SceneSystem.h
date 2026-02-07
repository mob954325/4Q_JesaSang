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

    void LoadSavedScenes();

	void BeforUpdate();
	void UpdateScene(float deltaTime);
	void FixedUpdateScene(float deltaTime);
	void LateUpdateScene(float deltaTime);

	std::shared_ptr<Scene> GetSceneByIndex(int index);
	void AddScene();
	void AddScene(const std::shared_ptr<Scene>& scene);
	std::shared_ptr<Scene> GetCurrentScene();

    int GetSceneCount();
	void ChangeScene(const std::shared_ptr<Scene>& scene);
	void ChangeSceneByIndex(int index);

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
