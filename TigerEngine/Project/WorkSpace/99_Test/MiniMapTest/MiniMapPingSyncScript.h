#pragma once

#include "Components/ScriptComponent.h"

class RectTransform;
class MiniMapTestScript;
class GameObject;

class MiniMapPingSyncScript : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    MiniMapPingSyncScript() { SetName("MiniMapPingSyncScript"); }
    ~MiniMapPingSyncScript() override = default;

    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    std::string playerObjectName = "Player";
    std::string baseMapObjectName = "UI_MiniMap_Base";
    std::string pingObjectName = "UI_MiniMap_MainPing";

private:
    MiniMapTestScript* m_Map = nullptr;
    RectTransform* m_BaseRect = nullptr;
    RectTransform* m_PingRect = nullptr;
};
