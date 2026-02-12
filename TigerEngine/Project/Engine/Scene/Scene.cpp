#include "Scene.h"	
#include "../Object/GameObject.h"
#include "../EngineSystem/ScriptSystem.h"
#include "../Manager/ComponentFactory.h"
#include "../Manager/WorldManager.h"
#include "System/ObjectSystem.h"
#include "../EngineSystem/LightSystem.h"
#include "../Components/Camera.h"
#include "../EngineSystem/CameraSystem.h"
#include "../EngineSystem/SceneSystem.h"

#include <algorithm>
#include <chrono>
#include <unordered_map>

void Scene::OnUpdate(float deltaTime)
{
    ScriptSystem::Instance().Update(deltaTime);
}

void Scene::OnFixedUpdate(float dt)
{
    ScriptSystem::Instance().FixedUpdate(dt);
}

void Scene::OnLateUpdate(float dt)
{
    ScriptSystem::Instance().LateUpdate(dt);
}

void Scene::CheckDestroy()
{
    for (auto it = gameObjects.begin(); it != gameObjects.end();)
    {
        auto entity = *it;
        if (entity.objPtr->IsDestory())
        {
            // 맵 요소 제거
            auto& container = mappedGameObjects[entity.objPtr->GetName()];
            for (auto mapContainerIt = container.begin(); mapContainerIt != container.end(); mapContainerIt++)
            {
                auto& [handle, index] = *mapContainerIt;
                if (entity.handle.generation == handle.generation &&
                    entity.handle.index == handle.index) // 같은 핸들 찾기
                {
                    container.erase(mapContainerIt);
                    break;
                }
            }

            // 벡터 요소 제거
            entity.objPtr->ClearAll();
            ObjectSystem::Instance().Destory(entity.handle);

            it = gameObjects.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void Scene::ForEachGameObject(std::function<void(GameObject*)> fn)
{
    for (auto& obj : gameObjects)
    {
        fn(obj.objPtr);
    }
}

GameObject* Scene::AddGameObjectByName(std::string name)
{
    Handle handle = ObjectSystem::Instance().Create<GameObject>();
    auto obj = ObjectSystem::Instance().Get<GameObject>(handle);
    obj->SetScene(this);
    obj->SetName(name);

    int index = static_cast<int>(gameObjects.size());
    gameObjects.push_back({ obj, handle });
    mappedGameObjects[name].push_back({ handle, index });
    obj->SetActive(true);

    return obj;
}

GameObject* Scene::GetGameObjectByName(std::string name)
{
    // 이름이 변하지 않았으면 map에서 찾기
    if (auto it = mappedGameObjects.find(name); it != mappedGameObjects.end())
    {
        auto container = it->second;
        return gameObjects[container.front().second].objPtr; // 무조건 첫번째 반환
    }

    //NOTE : 이름을 바꾸면 찾을 수 없음.
    // 따라서 그냥 GameObjects를 for문으로 돌면서 찾기
    for (auto it = gameObjects.begin(); it != gameObjects.end();)
    {
        if ((*it).objPtr->name == name)
        {
            return (*it).objPtr; // 같은 이름 중 첫번째로 온 오브젝트 반환
        }
        else
        {
            it++;
        }
    }

    return nullptr;
}

std::vector<GameObject*> Scene::GetGameObjectsByName(const std::string& name)
{
    std::vector<GameObject*> result;

    // 1) map에 있으면 빠르게 반환
    if (auto it = mappedGameObjects.find(name); it != mappedGameObjects.end())
    {
        const auto& container = it->second;

        result.reserve(container.size());
        for (const auto& [handle, index] : container)
        {
            // index 유효성 체크
            if (index < 0 || index >= static_cast<int>(gameObjects.size()))
                continue;

            GameObject* ptr = gameObjects[index].objPtr;
            if (!ptr) continue;

            // 혹시 이름 바뀌었거나 인덱스가 꼬였으면 걸러냄
            if (ptr->GetName() == name)
                result.push_back(ptr);
        }

        // map이 있는데 결과가 비어있으면(인덱스 꼬임 등) fallback으로 전체 탐색
        if (!result.empty())
            return result;
    }

    // 2) fallback: 전체 순회 (이름 변경 / 매핑 깨짐 대비)
    for (auto& entity : gameObjects)
    {
        if (!entity.objPtr) continue;
        if (entity.objPtr->GetName() == name)
            result.push_back(entity.objPtr);
    }

    return result;
}

GameObject* Scene::GetGameObject(GameObject* obj)
{
    std::string name = obj->GetName();
    if (auto it = mappedGameObjects.find(name); it != mappedGameObjects.end())
    {
        auto container = it->second;
        for (auto& [handle, index] : container)
        {
            if (gameObjects[index].objPtr->GetName() == name)
            {
                return gameObjects[index].objPtr;
            }
        }
    }
    else
    {
        return nullptr; // 없음
    }
}

void Scene::ClearScene()
{
    for (auto it = gameObjects.begin(); it != gameObjects.end();)
    {
        auto [ptr, handle] = *it;

        ptr->ClearAll();
        ObjectSystem::Instance().Destory(handle);
        it = gameObjects.erase(it);
    }

    gameObjects.clear();
    mappedGameObjects.clear();
    LightSystem::Instance().Clear();
    CameraSystem::Instance().Clear();
    ScriptSystem::Instance().Clear();
    RenderSystem::Instance().Clear();
}

bool Scene::SaveToJson(const std::string& filename) const
{
    nlohmann::json root;

    // 씬에 있는 게임 오브젝트 내용 저장
    root["objects"] = nlohmann::json::array();
    for (auto& entity : gameObjects)
    {
        if (!entity.objPtr) continue;

        nlohmann::json entityData = entity.objPtr->Serialize();
        root["objects"].push_back(entityData);
    }

    // 해당 씬의 월드 세팅 내용 저장
    root["worldData"] = nlohmann::json::array();
    auto& wm = WorldManager::Instance();
    nlohmann::json worldData = wm.Serialize();
    root["worldData"].push_back(worldData);

    // 파일 만들기
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << root.dump(2); // 보기 좋게 2칸 들여쓰기
    file.close();

    return true;
}

bool Scene::LoadToJson(const std::string& filename)
{
    if (filename.empty()) return false;

    checkSceneChange = true;
    targetLoadJsonPath = filename;

    return true;
}

bool Scene::CheckLoadJson()
{
    auto ResetLoadState = [&]()
    {
        loadRoot = nlohmann::json();
        loadCursor = 0;
        loadTotal = 0;
        loadParentIDs.clear();
        loadActiveFlags.clear();
        loadingJsonPath.clear();
        loadPhase = SceneLoadPhase::None;
    };

    auto DestroyLoadingCamera = [&]()
    {
        if (!hasLoadingCamera) return;

        if (auto* obj = ObjectSystem::Instance().Get<GameObject>(loadingCameraHandle))
        {
            obj->ClearAll();
        }
        ObjectSystem::Instance().Destory(loadingCameraHandle);

        loadingCameraHandle = {};
        hasLoadingCamera = false;
    };

    auto CancelOngoingLoad = [&]()
    {
        // NOTE : ClearOldScene 단계 전에 취소하면 기존 씬을 유지해야 한다.
        if (loadPhase == SceneLoadPhase::CreateObjects ||
            loadPhase == SceneLoadPhase::ApplyWorldData ||
            loadPhase == SceneLoadPhase::ActivateObjects ||
            loadPhase == SceneLoadPhase::ApplyHierarchy ||
            loadPhase == SceneLoadPhase::FixObjectIds)
        {
            ClearScene();
        }

        DestroyLoadingCamera();
        ResetLoadState();
    };

    auto FailLoad = [&](const char* msg)
    {
        if (msg)
        {
            MessageBoxA(nullptr, msg, "Error", MB_OK | MB_ICONERROR);
        }
        CancelOngoingLoad();
        return false;
    };

    // 새 요청이 들어오면 현재 로딩 취소 후 재시작
    if (checkSceneChange && loadPhase != SceneLoadPhase::None)
    {
        CancelOngoingLoad();
    }

    // Idle 상태에서만 요청을 받아 로딩 시작
    if (loadPhase == SceneLoadPhase::None)
    {
        if (!checkSceneChange) return false;

        if (targetLoadJsonPath.empty())
        {
            checkSceneChange = false;
            return false;
        }

        checkSceneChange = false;
        loadingJsonPath = targetLoadJsonPath;
        targetLoadJsonPath.clear();

        loadRoot = nlohmann::json();
        loadCursor = 0;
        loadTotal = 0;
        loadParentIDs.clear();
        loadActiveFlags.clear();

        loadPhase = SceneLoadPhase::ParseJson;
    }

    for (int safety = 0; safety < 16; ++safety)
    {
        switch (loadPhase)
        {
        case SceneLoadPhase::ParseJson:
        {
            std::ifstream file(loadingJsonPath);
            if (!file.is_open())
            {
                return FailLoad(nullptr);
            }

            try
            {
                file >> loadRoot;
            }
            catch (const nlohmann::json::exception& e)
            {
                file.close();
                return FailLoad(e.what());
            }
            file.close();

            if (!loadRoot.contains("objects") || !loadRoot["objects"].is_array())
            {
                return FailLoad("Invalid scene json: objects[] not found.");
            }

            loadTotal = loadRoot["objects"].size();
            loadCursor = 0;
            loadParentIDs.clear();
            loadActiveFlags.clear();

            loadPhase = SceneLoadPhase::ClearOldScene;
            continue;
        }

        case SceneLoadPhase::ClearOldScene:
        {
            // 이제부터는 기존 씬을 지우고 로딩 카메라를 유지하면서 점진적으로 생성한다.
            ClearScene();
            targetLoadedPath = loadingJsonPath;
            SceneSystem::Instance().isSceneChanging = true;

            // Loading camera (Release에서 FreeCamera가 없을 수 있어 크래시 방지)
            if (!hasLoadingCamera)
            {
                loadingCameraHandle = ObjectSystem::Instance().Create<GameObject>();
                if (auto* loadingObj = ObjectSystem::Instance().Get<GameObject>(loadingCameraHandle))
                {
                    loadingObj->SetName("LoadingCamera");
                    loadingObj->SetScene(this);
                    // NOTE : GameObject는 비활성화 상태로 두고, Camera 컴포넌트만 활성화해서
                    // ScriptSystem(Transform 포함) 등록을 피한다.
                    if (auto* loadingCam = loadingObj->AddComponent<Camera>())
                    {
                        loadingCam->SetActive(true);
                        hasLoadingCamera = true;
                    }
                }
            }

            loadPhase = SceneLoadPhase::CreateObjects;
            continue;
        }

        case SceneLoadPhase::CreateObjects:
        {
            SceneSystem::Instance().isSceneChanging = true;

            constexpr auto kLoadTimeBudget = std::chrono::milliseconds(3);
            constexpr size_t kMinObjectsPerTick = 1;
            constexpr size_t kMaxObjectsPerTick = 128;

            const auto tickStart = std::chrono::steady_clock::now();
            size_t createdThisTick = 0;

            auto CreateGameObjectInactive = [&](const std::string& name) -> GameObject*
            {
                Handle handle = ObjectSystem::Instance().Create<GameObject>();
                auto* obj = ObjectSystem::Instance().Get<GameObject>(handle);
                if (!obj) return nullptr;

                obj->SetScene(this);
                obj->SetName(name);

                const int index = static_cast<int>(gameObjects.size());
                gameObjects.push_back({ obj, handle });
                mappedGameObjects[name].push_back({ handle, index });

                // NOTE : 로딩 중에는 비활성화 상태로 유지 (추후 ActivateObjects 단계에서 켠다)
                return obj;
            };

            while (loadCursor < loadTotal)
            {
                if (createdThisTick >= kMaxObjectsPerTick) break;
                if (createdThisTick >= kMinObjectsPerTick &&
                    (std::chrono::steady_clock::now() - tickStart) >= kLoadTimeBudget)
                {
                    break;
                }

                const auto& objData = loadRoot["objects"][loadCursor];
                loadCursor++;

                if (!objData.contains("properties") || !objData["properties"].is_object())
                {
                    continue;
                }

                const auto& propsRef = objData["properties"];

                std::string objectName;
                if (propsRef.contains("Name") && propsRef["Name"].is_string())
                {
                    objectName = propsRef["Name"].get<std::string>();
                }

                auto* instance = CreateGameObjectInactive(objectName);
                if (!instance) continue;

                // Parent/Active 정보 저장
                int parentID = -1;
                if (propsRef.contains("ParentID") && propsRef["ParentID"].is_number_integer())
                {
                    parentID = propsRef["ParentID"].get<int>();
                }
                loadParentIDs.push_back(parentID);

                bool isActive = true;
                if (propsRef.contains("Active") && propsRef["Active"].is_boolean())
                {
                    isActive = propsRef["Active"].get<bool>();
                }
                loadActiveFlags.push_back(isActive);

                // Deserialize는 components만 필요. Active는 포함하지 않아 활성화가 일어나지 않게 한다.
                nlohmann::json props = nlohmann::json::object();
                if (propsRef.contains("components") && propsRef["components"].is_array())
                {
                    props["components"] = propsRef["components"];
                }

                // ComponentFactory가 생성 직후 SetActive(true)를 호출하므로, 로딩 중엔 이를 억제한다.
                auto& factory = ComponentFactory::Instance();
                const bool prevSuppress = factory.suppressAutoActivate;
                factory.suppressAutoActivate = true;
                instance->Deserialize(props);
                factory.suppressAutoActivate = prevSuppress;

                // ID는 계층 구성용으로만 유지 (완료 후 재부여)
                if (propsRef.contains("ID") && propsRef["ID"].is_number_integer())
                {
                    instance->SetId(propsRef["ID"].get<int>());
                }

                createdThisTick++;
            }

            if (loadCursor < loadTotal)
            {
                return true; // 다음 프레임에 계속 생성
            }

            loadPhase = SceneLoadPhase::ApplyWorldData;
            continue;
        }

        case SceneLoadPhase::ApplyWorldData:
        {
            SceneSystem::Instance().isSceneChanging = true;

            // worldData는 없는 경우도 있을 수 있으므로 optional로 처리
            if (loadRoot.contains("worldData") && loadRoot["worldData"].is_array() && !loadRoot["worldData"].empty())
            {
                auto& wm = WorldManager::Instance();
                wm.Deserialize(loadRoot["worldData"][0]);
            }

            loadPhase = SceneLoadPhase::ActivateObjects;
            continue;
        }

        case SceneLoadPhase::ActivateObjects:
        {
            SceneSystem::Instance().isSceneChanging = true;

            const size_t count = std::min(gameObjects.size(), loadActiveFlags.size());
            for (size_t i = 0; i < count; ++i)
            {
                if (!gameObjects[i].objPtr) continue;
                gameObjects[i].objPtr->SetActive(loadActiveFlags[i]);
            }

            loadPhase = SceneLoadPhase::ApplyHierarchy;
            continue;
        }

        case SceneLoadPhase::ApplyHierarchy:
        {
            SceneSystem::Instance().isSceneChanging = true;

            std::unordered_map<int, Transform*> idToTransform;
            idToTransform.reserve(gameObjects.size());

            for (auto& entity : gameObjects)
            {
                if (!entity.objPtr) continue;
                idToTransform[entity.objPtr->GetId()] = entity.objPtr->GetTransform();
            }

            const size_t count = std::min(gameObjects.size(), loadParentIDs.size());
            for (size_t i = 0; i < count; ++i)
            {
                auto* obj = gameObjects[i].objPtr;
                if (!obj) continue;

                const int parentID = loadParentIDs[i];
                if (parentID == -1) continue;

                auto it = idToTransform.find(parentID);
                if (it == idToTransform.end()) continue;

                obj->GetTransform()->SetParent(it->second);
            }

            loadPhase = SceneLoadPhase::FixObjectIds;
            continue;
        }

        case SceneLoadPhase::FixObjectIds:
        {
            SceneSystem::Instance().isSceneChanging = true;

            for (auto& obj : gameObjects)
            {
                if (!obj.objPtr) continue;
                obj.objPtr->SetId(ObjectSystem::Instance().GetNewID());
            }

            // 로딩 카메라는 다른 카메라가 존재할 때만 제거 (없으면 Release에서 크래시 위험)
            if (hasLoadingCamera)
            {
                bool hasOtherCamera = false;
                for (auto* cam : CameraSystem::Instance().GetAllCamera())
                {
                    if (!cam || !cam->GetOwner()) continue;
                    if (cam->GetOwner()->GetName() == "LoadingCamera") continue;

                    hasOtherCamera = true;
                    break;
                }

                if (hasOtherCamera)
                {
                    DestroyLoadingCamera();
                }
            }

            ResetLoadState();
            SceneSystem::Instance().isSceneChanging = false; // 로딩 완료 프레임에는 정상 렌더링
            return true;
        }

        case SceneLoadPhase::None:
        default:
            return false;
        }
    }

    return true;
}

GameObject* Scene::RayCastGameObject(const Ray& ray, float* outDistance)
{
    GameObject* hitObject = nullptr;
    float minDistant = FLT_MAX;

    for (auto& [ptr, handle] : gameObjects)
    {
        if (!ptr) continue;

        float outDist = 0.0f;
        if (ray.Intersects(ptr->GetAABB(), outDist))
        {
            if (outDist < minDistant)
            {
                minDistant = outDist;
                hitObject = ptr;
            }
        }
    }

    if (outDistance != nullptr && hitObject)
    {
        *outDistance = minDistant;
    }

    return hitObject;
}

void Scene::ReloadScene()
{
    if (targetLoadedPath.empty()) return;

    LoadToJson(targetLoadedPath);
}

int Scene::GetGameObjectIndex(GameObject* ptr)
{
    int res = -1;
    for (int i = 0; i < gameObjects.size(); i++)
    {
        auto [objPtr, handle] = gameObjects[i];

        if (ptr == objPtr)
        {
            res = i;
            break;
        }
    }

    return res;
}

GameObject* Scene::GetGameObjectByIndex(int index)
{
    if (index < 0 || index >= gameObjects.size()) return nullptr;

    return gameObjects[index].objPtr;
}
