#include "SceneSystem.h"
#include "../Util/PathHelper.h"

namespace fs = std::filesystem;

void SceneSystem::LoadSavedScenes()
{
    auto sceneDir = PathHelper::FindDirectory("Assets/Scenes");
    if (!sceneDir) return;

    std::vector<fs::path> sceneFiles;
    for (const auto& entry : fs::directory_iterator(*sceneDir))
    {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() == ".json")
        {
            sceneFiles.push_back(entry.path());
        }
    }

    for (const auto& path : sceneFiles)
    {
        auto scene = std::make_shared<Scene>();
        if (!scene->LoadToJson(path.string())) continue;
        AddScene(scene);
    }
}

void SceneSystem::CheckSceneChange()
{
    if (nextSceneIndex != currSceneIndex) // 씬 전환 감지
    {
        auto it = scenes.find(nextSceneIndex);
        if (it == scenes.end()) return;

        if (currentScene)
            currentScene->CloseScene();

        currentScene = it->second;
        currentScene->OpenScene();

        currSceneIndex = nextSceneIndex;
    }
}

void SceneSystem::BeforUpdate()
{
	if(scenes.empty()) return;

	currentScene->CheckDestroy();
}

void SceneSystem::UpdateScene(float deltaTime)
{
	if(scenes.empty()) return; // 씬 없어서 터지는거 방지

	currentScene->OnUpdate(deltaTime);
}

void SceneSystem::FixedUpdateScene(float deltaTime)
{
    if (scenes.empty()) return; // 씬 없어서 터지는거 방지

    currentScene->OnFixedUpdate(deltaTime);
}

void SceneSystem::LateUpdateScene(float deltaTime)
{
    if (scenes.empty()) return; // 씬 없어서 터지는거 방지

    currentScene->OnLateUpdate(deltaTime);
}

std::shared_ptr<Scene> SceneSystem::GetSceneByIndex(int index)
{
	return scenes.find(index)->second;
}

void SceneSystem::AddScene()
{
    AddScene(std::make_shared<Scene>());
}

void SceneSystem::AddScene(const std::shared_ptr<Scene>& scene)
{
    if (!scene) return;
    scenes.insert({static_cast<int>(scenes.size()), scene});
    if (!currentScene) currentScene = scene;
    scene->CloseScene();
}

std::shared_ptr<Scene> SceneSystem::GetCurrentScene()
{
    return currentScene;
}

int SceneSystem::GetSceneCount()
{
    return static_cast<int>(scenes.size());
}

void SceneSystem::ChangeSceneByIndex(int index)
{
	if (index < 0 || index >= static_cast<int>(scenes.size())) return;

    nextSceneIndex = index; 
}
