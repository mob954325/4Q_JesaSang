#include "MiniMapPingSyncScript.h"

#include "EngineSystem/SceneSystem.h"
#include "Components/RectTransform.h"
#include "Object/GameObject.h"
#include "Util/ComponentAutoRegister.h"
#include "Util/JsonHelper.h"
#include "MiniMapTestScript.h"

REGISTER_COMPONENT(MiniMapPingSyncScript)

RTTR_REGISTRATION
{
    rttr::registration::class_<MiniMapPingSyncScript>("MiniMapPingSyncScript")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("PlayerObjectName", &MiniMapPingSyncScript::playerObjectName)
        .property("BaseMapObjectName", &MiniMapPingSyncScript::baseMapObjectName)
        .property("PingObjectName", &MiniMapPingSyncScript::pingObjectName);
}

void MiniMapPingSyncScript::OnInitialize()
{
}

void MiniMapPingSyncScript::OnStart()
{
    auto scene = SceneSystem::Instance().GetCurrentScene();
    if (!scene)
    {
        return;
    }

    if (auto player = scene->GetGameObjectByName(playerObjectName))
    {
        m_Map = player->GetComponent<MiniMapTestScript>();
    }

    if (auto baseMap = scene->GetGameObjectByName(baseMapObjectName))
    {
        m_BaseRect = baseMap->GetComponent<RectTransform>();
    }

    if (auto ping = scene->GetGameObjectByName(pingObjectName))
    {
        m_PingRect = ping->GetComponent<RectTransform>();
    }
}

void MiniMapPingSyncScript::OnUpdate(float delta)
{
    (void)delta;

    if (!m_Map || !m_BaseRect || !m_PingRect)
    {
        return;
    }

    const Vector2 local = m_Map->GetPlayerMapPos();
    const Vector3 basePos = m_BaseRect->GetPos();
    const Vector3 pingPos(basePos.x + local.x, basePos.y + local.y, basePos.z);
    m_PingRect->SetPos(pingPos);
}

nlohmann::json MiniMapPingSyncScript::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void MiniMapPingSyncScript::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}
