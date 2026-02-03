#pragma once
#include "../../Base/pch.h"
#include "../Object/GameObject.h"
#include "../EngineSystem/SceneSystem.h"
#include "PathHelper.h"

class PrefabUtil
{
public:
    static GameObject* Instantiate(const std::string& fileName)
    {
        auto prefabsDir = PathHelper::FindDirectory("Assets\\Prefabs");
        if (!prefabsDir.has_value()) return nullptr; // 폴더 없음
        std::string path = prefabsDir.value().string();
        path += "\\" + fileName + ".json";

        std::ifstream file(path);
        if (!file.is_open()) return nullptr;

        nlohmann::json root;
        try
        {
            file >> root;
        }
        catch (const nlohmann::json::exception&)
        {
            file.close();
            return nullptr;
        }
        file.close();

        // 저장 포맷: { "prefab": [ {properties...}, {properties...}, ... ] }
        if (!root.contains("prefab") || !root["prefab"].is_array())
            return nullptr;

        auto currScene = SceneSystem::Instance().GetCurrentScene();
        const auto& registered = ComponentFactory::Instance().GetRegisteredComponents();

        if (!currScene) return nullptr;

        if (fileName.empty()) return nullptr;

        std::vector<GameObject*> created;
        std::vector<int> parentIDs;      // 생성 순서대로 ParentID 저장 (-1 가능)

        // 1) 전부 생성 + Deserialize + ID/ParentID 수집
        for (const auto& objData : root["prefab"])
        {
            // 이름 찾기
            std::string objectName = "";
            if (objData.contains("Name")) objectName = objData["Name"];

            auto instance = currScene->AddGameObjectByName(objectName);
            created.push_back(instance);
            if (!instance) continue;

            // 저장된 id 등록
            if (objData.contains("ID"))
            {
                instance->SetId(objData["ID"]); // ID 추가
            }

            // 부모 id 등록
            if (objData.contains("ParentID"))
            {
                parentIDs.push_back(objData["ParentID"]); // 미리 부모 ID 기억
            }

            // 컴포넌트 등록하기
            for (const auto& comp : objData["components"])
            {
                if (!comp.contains("type")) continue;
                if (comp["type"] == "Transform") continue; // transform이면 무시하기 ( 이미 위에서 등록 )


                auto it = registered.find(comp["type"]); // 등록된 컴포넌트 찾기
                if (it != registered.end())
                {
                    Component* createdComp = it->second.creator(instance);
                    createdComp->Deserialize(comp);
                }
            }
        }

        if (created.empty()) return nullptr;

        // 2) 계층 재구성 (씬 로드 로직과 동일)
        // created[i] <-> parentIDs[i] 1:1 매칭
        if (!parentIDs.empty() && parentIDs.size() == created.size())
        {
            for (int i = 0; i < (int)created.size(); ++i)
            {
                int currParentID = parentIDs[i];
                if (currParentID == -1) continue;

                GameObject* currObject = created[i];
                if (!currObject) continue;

                for (int j = 0; j < (int)created.size(); ++j)
                {
                    if (i == j) continue;
                    if (!created[j]) continue;

                    if (currParentID == created[j]->GetId())
                    {
                        currObject->GetTransform()->SetParent(created[j]->GetTransform());
                        break;
                    }
                }
            }
        }

        // 3-1) 구성 후 ObjID 충돌 방지를 위한 ID 재구성
        for (int i = 0; i < (int)created.size(); ++i)
        {
            if (!created[i]) continue;

            created[i]->SetId(ObjectSystem::Instance().GetNewID()); // 새 ID 갱신
        }

        // 4) 반환값: 첫 번째 루트(ParentID == -1)를 대표로 반환
        for (int i = 0; i < (int)created.size(); ++i)
        {
            if (created[i] && parentIDs[i] == -1)
                return created[i];
        }
    }
};
