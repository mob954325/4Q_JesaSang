#include "MiniMapManager.h"

#include "EngineSystem/SceneSystem.h"
#include "Components/RectTransform.h"
#include "Components/UI/Image.h"
#include "Manager/UIManager.h"
#include "Object/GameObject.h"
#include "Util/ComponentAutoRegister.h"
#include "Util/JsonHelper.h"
#include "MiniMapTestScript.h"

#include <algorithm>
#include <iostream>

REGISTER_COMPONENT(MiniMapManager)

RTTR_REGISTRATION
{
    rttr::registration::class_<MiniMapManager>("MiniMapManager")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("PlayerObjectName", &MiniMapManager::playerObjectName)
        .property("BaseMapObjectName", &MiniMapManager::baseMapObjectName)
        .property("CompleteMapObjectName", &MiniMapManager::completeMapObjectName)
        .property("MainPingObjectName", &MiniMapManager::mainPingObjectName)
        .property("GaugeObjectName", &MiniMapManager::gaugeObjectName)
        .property("GaugeBgObjectName", &MiniMapManager::gaugeBgObjectName)
        .property("BaseMapPath", &MiniMapManager::baseMapPath)
        .property("CompleteMapPath", &MiniMapManager::completeMapPath)
        .property("PingObjectName1", &MiniMapManager::itemPingObjectName1)
        .property("PingObjectName2", &MiniMapManager::itemPingObjectName2)
        .property("PingObjectName3", &MiniMapManager::itemPingObjectName3)
        .property("PingObjectName4", &MiniMapManager::itemPingObjectName4)
        .property("PingObjectName5", &MiniMapManager::itemPingObjectName5)
        .property("PingObjectName6", &MiniMapManager::itemPingObjectName6)
        .property("PieceObjectName1", &MiniMapManager::pieceObjectName1)
        .property("PieceObjectName2", &MiniMapManager::pieceObjectName2)
        .property("PieceObjectName3", &MiniMapManager::pieceObjectName3)
        .property("PieceObjectName4", &MiniMapManager::pieceObjectName4)
        .property("PieceObjectName5", &MiniMapManager::pieceObjectName5)
        .property("PieceObjectName6", &MiniMapManager::pieceObjectName6)
        .property("ItemWorldPos1", &MiniMapManager::itemWorldPos1)
        .property("ItemWorldPos2", &MiniMapManager::itemWorldPos2)
        .property("ItemWorldPos3", &MiniMapManager::itemWorldPos3)
        .property("ItemWorldPos4", &MiniMapManager::itemWorldPos4)
        .property("ItemWorldPos5", &MiniMapManager::itemWorldPos5)
        .property("ItemWorldPos6", &MiniMapManager::itemWorldPos6)
        .property("DebugShowAllItemPings", &MiniMapManager::debugShowAllItemPings)
        .property("DebugShowAllPieces", &MiniMapManager::debugShowAllPieces)
        .property("PlaceBottomRight", &MiniMapManager::placeBottomRight)
        .property("BottomRightMargin", &MiniMapManager::bottomRightMargin)
        .property("ShiftLeftByMapCount", &MiniMapManager::shiftLeftByMapCount)
        .property("ShowCompleteOnFull", &MiniMapManager::showCompleteOnFull);
}

static Vector2 WorldToMiniMap(const Vector3& worldPos, const Vector3& worldMin, const Vector3& worldMax, const Vector2& mapSize)
{
    const float dx = worldMax.x - worldMin.x;
    const float dz = worldMax.z - worldMin.z;
    const float nx = (dx != 0.0f) ? (worldPos.x - worldMin.x) / dx : 0.0f;
    const float nz = (dz != 0.0f) ? (worldPos.z - worldMin.z) / dz : 0.0f;

    Vector2 out;
    out.x = std::clamp(nx, 0.0f, 1.0f) * mapSize.x;
    out.y = (1.0f - std::clamp(nz, 0.0f, 1.0f)) * mapSize.y;
    return out;
}

static void LogMissing(const char* label, const std::string& name)
{
    std::cout << "[MiniMapManager] Missing " << label << ": " << name << std::endl;
}

static RectTransform* GetRectOrLog(const std::shared_ptr<Scene>& scene, const std::string& name, const char* label)
{
    if (!scene)
    {
        LogMissing("scene", "null");
        return nullptr;
    }
    if (auto obj = scene->GetGameObjectByName(name))
    {
        auto rect = obj->GetComponent<RectTransform>();
        if (!rect) LogMissing(label, name);
        return rect;
    }
    LogMissing(label, name);
    return nullptr;
}

static Image* GetImage(const std::shared_ptr<Scene>& scene, const std::string& name)
{
    if (!scene)
    {
        return nullptr;
    }
    if (auto obj = scene->GetGameObjectByName(name))
    {
        return obj->GetComponent<Image>();
    }
    return nullptr;
}

static Image* GetImageOrLog(const std::shared_ptr<Scene>& scene, const std::string& name, const char* label)
{
    if (!scene)
    {
        LogMissing("scene", "null");
        return nullptr;
    }
    if (auto obj = scene->GetGameObjectByName(name))
    {
        auto image = obj->GetComponent<Image>();
        if (!image) LogMissing(label, name);
        return image;
    }
    LogMissing(label, name);
    return nullptr;
}

static void SetRectActive(RectTransform* rect, bool active)
{
    if (rect)
    {
        rect->SetActive(active);
    }
}

static void SetImageActive(Image* image, bool active)
{
    if (image)
    {
        image->SetActive(active);
    }
}

static void SetImageAlpha(Image* image, float alpha)
{
    if (!image) return;
    Color c = image->GetColor();
    c.w = std::clamp(alpha, 0.0f, 1.0f);
    image->SetColor(c);
}

static void BumpRenderOrder(Image* image)
{
    if (!image || !image->GetActiveSelf())
    {
        return;
    }

    image->SetActive(false);
    image->SetActive(true);
}

static int ClampItemIndex(int index)
{
    if (index < 0) return 0;
    if (index > 5) return 5;
    return index;
}

static constexpr float kPingZOffset = 2.0f;
static constexpr float kPieceZOffset = -1.0f;
static constexpr float kCompleteZOffset = -2.0f;

static void InitActive(RectTransform* rect, Image* image, bool active)
{
    SetRectActive(rect, active);
    SetImageActive(image, active);
}

static void SetZ(RectTransform* rect, float z)
{
    if (!rect) return;
    Vector3 pos = rect->GetPos();
    pos.z = z;
    rect->SetPos(pos);
}

static void ShiftRect(RectTransform* rect, const Vector3& delta)
{
    if (!rect) return;
    Vector3 pos = rect->GetPos();
    pos.x += delta.x;
    pos.y += delta.y;
    pos.z += delta.z;
    rect->SetPos(pos);
}

void MiniMapManager::OnInitialize()
{
}

void MiniMapManager::OnStart()
{
    auto scene = SceneSystem::Instance().GetCurrentScene();
    if (!scene)
    {
        return;
    }

    if (auto player = scene->GetGameObjectByName(playerObjectName))
    {
        m_Map = player->GetComponent<MiniMapTestScript>();
        if (!m_Map) LogMissing("MiniMapTestScript on player", playerObjectName);
    }
    else
    {
        LogMissing("player object", playerObjectName);
    }

    if (auto obj = scene->GetGameObjectByName(baseMapObjectName))
    {
        m_BaseObject = obj;
    }
    if (auto obj = scene->GetGameObjectByName(completeMapObjectName))
    {
        m_CompleteObject = obj;
    }

    m_BaseRect = GetRectOrLog(scene, baseMapObjectName, "RectTransform on base map object");
    m_BaseImage = GetImage(scene, baseMapObjectName);
    m_CompleteRect = GetRectOrLog(scene, completeMapObjectName, "RectTransform on complete map object");
    m_CompleteImage = GetImage(scene, completeMapObjectName);
    m_MainPingRect = GetRectOrLog(scene, mainPingObjectName, "RectTransform on main ping object");
    m_MainPingImage = GetImage(scene, mainPingObjectName);
    m_GaugeRect = GetRectOrLog(scene, gaugeObjectName, "RectTransform on gauge object");
    m_GaugeBgRect = GetRectOrLog(scene, gaugeBgObjectName, "RectTransform on gauge bg object");
    if (m_GaugeRect)
    {
        m_GaugeBaseSize = m_GaugeRect->GetSize();
        m_HasGaugeBaseSize = true;
    }

    const char* pingNames[6] = {
        itemPingObjectName1.c_str(), itemPingObjectName2.c_str(), itemPingObjectName3.c_str(),
        itemPingObjectName4.c_str(), itemPingObjectName5.c_str(), itemPingObjectName6.c_str()
    };

    for (int i = 0; i < 6; ++i)
    {
        m_ItemPingRects[i] = GetRectOrLog(scene, pingNames[i], "RectTransform on item ping object");
        m_ItemPingImages[i] = GetImage(scene, pingNames[i]);
    }

    const char* pieceNames[6] = {
        pieceObjectName1.c_str(), pieceObjectName2.c_str(), pieceObjectName3.c_str(),
        pieceObjectName4.c_str(), pieceObjectName5.c_str(), pieceObjectName6.c_str()
    };

    for (int i = 0; i < 6; ++i)
    {
        if (pieceNames[i] == nullptr || pieceNames[i][0] == '\0')
        {
            m_PieceRects[i] = nullptr;
            m_PieceImages[i] = nullptr;
            continue;
        }

        m_PieceRects[i] = GetRectOrLog(scene, pieceNames[i], "RectTransform on piece object");
        m_PieceImages[i] = GetImageOrLog(scene, pieceNames[i], "Image on piece object");
    }

    m_TreasureImage = GetImage(scene, "UI_MiniMap_Treasure");
    m_TreasureRect = GetRectOrLog(scene, "UI_MiniMap_Treasure", "RectTransform on treasure object");

    if (m_BaseImage)
    {
        SetImageActive(m_BaseImage, true);
        if (!baseMapPath.empty())
        {
            m_BaseImage->ChangeData(baseMapPath);
            m_UsingCompleteMap = false;
        }
        SetImageAlpha(m_BaseImage, 1.0f);
    }
    if (m_CompleteImage)
    {
        SetImageActive(m_CompleteImage, false);
        SetRectActive(m_CompleteRect, false);
        SetImageAlpha(m_CompleteImage, 0.0f);
    }
    if (m_BaseObject)
    {
        m_BaseObject->SetActive(true);
    }
    if (m_CompleteObject)
    {
        m_CompleteObject->SetActive(false);
    }

    ApplyLayout();

    m_ItemWorldPos[0] = itemWorldPos1;
    m_ItemWorldPos[1] = itemWorldPos2;
    m_ItemWorldPos[2] = itemWorldPos3;
    m_ItemWorldPos[3] = itemWorldPos4;
    m_ItemWorldPos[4] = itemWorldPos5;
    m_ItemWorldPos[5] = itemWorldPos6;

    for (int i = 0; i < 6; ++i)
    {
        m_ItemActive[i] = debugShowAllItemPings;
        SetRectActive(m_ItemPingRects[i], m_ItemActive[i]);
    }

    for (int i = 0; i < 6; ++i)
    {
        m_PieceActive[i] = debugShowAllPieces;
        InitActive(m_PieceRects[i], m_PieceImages[i], m_PieceActive[i]);
    }

    if (m_BaseRect)
    {
        const float baseZ = m_BaseRect->GetPos().z;
        SetZ(m_CompleteRect, baseZ + kCompleteZOffset);
        for (int i = 0; i < 6; ++i)
        {
            SetZ(m_PieceRects[i], baseZ + kPieceZOffset);
        }
    }
}

void MiniMapManager::OnUpdate(float delta)
{
    (void)delta;

    if (!m_Map || !m_BaseRect)
    {
        return;
    }

    if (!m_LayoutApplied)
    {
        ApplyLayout();
    }

    if (m_BaseImage)
    {
        const float progress = std::clamp(m_Map->GetProgress01(), 0.0f, 1.0f);
        const bool complete = showCompleteOnFull && progress >= 0.999f;
        const bool wantComplete = complete && !completeMapPath.empty();
        if (wantComplete != m_UsingCompleteMap)
        {
            m_BaseImage->ChangeData(wantComplete ? completeMapPath : baseMapPath);
            m_UsingCompleteMap = wantComplete;
        }
        SetImageActive(m_BaseImage, true);
        SetRectActive(m_BaseRect, true);
        SetImageAlpha(m_BaseImage, 1.0f);
        if (m_CompleteImage)
        {
            SetImageAlpha(m_CompleteImage, complete ? 1.0f : 0.0f);
        }
        if (m_CompleteObject) m_CompleteObject->SetActive(false);
    }

    const Vector3 basePos = m_BaseRect->GetPos();

    if (m_MainPingRect)
    {
        const Vector2 local = m_Map->GetPlayerMapPos();
        const Vector3 pingPos(basePos.x + local.x, basePos.y + local.y, basePos.z + kPingZOffset);
        m_MainPingRect->SetPos(pingPos);
    }

    const Vector3 worldMin = m_Map->GetWorldMin();
    const Vector3 worldMax = m_Map->GetWorldMax();
    const Vector2 mapSize = m_Map->GetMapSize();

    for (int i = 0; i < 6; ++i)
    {
        if (!m_ItemPingRects[i] || !m_ItemActive[i]) continue;

        const Vector2 local = WorldToMiniMap(m_ItemWorldPos[i], worldMin, worldMax, mapSize);
        const Vector3 pingPos(basePos.x + local.x, basePos.y + local.y, basePos.z + kPingZOffset);
        m_ItemPingRects[i]->SetPos(pingPos);
    }

    if (m_GaugeRect && m_HasGaugeBaseSize)
    {
        const float progress = std::clamp(m_Map->GetProgress01(), 0.0f, 1.0f);
        Vector2 size = m_GaugeBaseSize;
        size.x = m_GaugeBaseSize.x * progress;
        m_GaugeRect->SetSize(size);
    }
}

void MiniMapManager::ApplyLayout()
{
    if (!placeBottomRight || !m_BaseRect || m_LayoutApplied)
    {
        return;
    }

    const Vector2 screen = UIManager::Instance().GetSize();
    const Vector2 size = m_BaseRect->GetSize();
    if (screen.x <= 0.0f || screen.y <= 0.0f || size.x <= 0.0f || size.y <= 0.0f)
    {
        return;
    }

    const Vector3 current = m_BaseRect->GetPos();
    const Vector3 target(
        screen.x - size.x - bottomRightMargin.x - (size.x * static_cast<float>(shiftLeftByMapCount)),
        screen.y - size.y - bottomRightMargin.y,
        current.z
    );
    const Vector3 delta = target - current;

    ShiftRect(m_BaseRect, delta);
    ShiftRect(m_CompleteRect, delta);
    ShiftRect(m_GaugeRect, delta);
    ShiftRect(m_GaugeBgRect, delta);
    ShiftRect(m_MainPingRect, delta);
    ShiftRect(m_TreasureRect, delta);
    for (int i = 0; i < 6; ++i)
    {
        ShiftRect(m_ItemPingRects[i], delta);
        ShiftRect(m_PieceRects[i], delta);
    }
    m_LayoutApplied = true;
}

nlohmann::json MiniMapManager::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void MiniMapManager::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}

void MiniMapManager::TriggerItemPing(int index, const Vector3& worldPos)
{
    const int i = ClampItemIndex(index);
    m_ItemWorldPos[i] = worldPos;
    m_ItemActive[i] = true;
    SetRectActive(m_ItemPingRects[i], m_ItemActive[i]);
}

void MiniMapManager::TriggerItemPingActive(int index, bool active)
{
    const int i = ClampItemIndex(index);
    m_ItemActive[i] = active;
    SetRectActive(m_ItemPingRects[i], active);
}

void MiniMapManager::TriggerAllItemPingsInactive()
{
    for (int i = 0; i < 6; ++i)
    {
        m_ItemActive[i] = false;
        SetRectActive(m_ItemPingRects[i], m_ItemActive[i]);
    }
}

void MiniMapManager::TriggerPieceActive(int index, bool active)
{
    const int i = ClampItemIndex(index);
    m_PieceActive[i] = active;
    InitActive(m_PieceRects[i], m_PieceImages[i], m_PieceActive[i]);

    if (active)
    {
        BumpPingRenderOrder();
    }
}

void MiniMapManager::TriggerAllPiecesInactive()
{
    for (int i = 0; i < 6; ++i)
    {
        m_PieceActive[i] = false;
        InitActive(m_PieceRects[i], m_PieceImages[i], m_PieceActive[i]);
    }
}

void MiniMapManager::BumpPingRenderOrder()
{
    BumpRenderOrder(m_MainPingImage);
    BumpRenderOrder(m_TreasureImage);
    for (int i = 0; i < 6; ++i)
    {
        BumpRenderOrder(m_ItemPingImages[i]);
    }
}

void MiniMapManager::TriggerItemCollected(int index, const Vector3& worldPos)
{
    TriggerItemPing(index, worldPos);
    if (m_Map)
    {
        m_Map->TriggerItemCollected();
    }
}

void MiniMapManager::TriggerPieceCollected(int index)
{
    TriggerPieceActive(index, true);
    if (m_Map)
    {
        m_Map->TriggerPieceCollected();
    }
}
