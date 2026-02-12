#include "MiniMapManager.h"

#include "EngineSystem/SceneSystem.h"
#include "Components/RectTransform.h"
#include "Components/UI/Image.h"
#include "Manager/UIManager.h"
#include "Object/GameObject.h"
#include "Util/ComponentAutoRegister.h"
#include "Util/JsonHelper.h"
#include "MiniMapTestScript.h"
#include "../../Woo/Object/SearchObject.h"
#include "Components/Transform.h"

#include <algorithm>
#include <iostream>
#include <sstream>

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
        .property("ShrineMapPath", &MiniMapManager::shrineMapPath)
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
        .property("Zone0IngredientIds", &MiniMapManager::zone0IngredientIds)
        .property("Zone1IngredientIds", &MiniMapManager::zone1IngredientIds)
        .property("Zone2IngredientIds", &MiniMapManager::zone2IngredientIds)
        .property("Zone3IngredientIds", &MiniMapManager::zone3IngredientIds)
        .property("Zone4IngredientIds", &MiniMapManager::zone4IngredientIds)
        .property("Zone0PingIndices", &MiniMapManager::zone0PingIndices)
        .property("Zone1PingIndices", &MiniMapManager::zone1PingIndices)
        .property("Zone2PingIndices", &MiniMapManager::zone2PingIndices)
        .property("Zone3PingIndices", &MiniMapManager::zone3PingIndices)
        .property("Zone4PingIndices", &MiniMapManager::zone4PingIndices)
        .property("ItemWorldPos1", &MiniMapManager::itemWorldPos1)
        .property("ItemWorldPos2", &MiniMapManager::itemWorldPos2)
        .property("ItemWorldPos3", &MiniMapManager::itemWorldPos3)
        .property("ItemWorldPos4", &MiniMapManager::itemWorldPos4)
        .property("ItemWorldPos5", &MiniMapManager::itemWorldPos5)
        .property("ItemWorldPos6", &MiniMapManager::itemWorldPos6)
        .property("DebugShowAllItemPings", &MiniMapManager::debugShowAllItemPings)
        .property("DebugShowAllPieces", &MiniMapManager::debugShowAllPieces)
        .property("UseFixedItemPingPositions", &MiniMapManager::useFixedItemPingPositions)
        .property("PlaceBottomRight", &MiniMapManager::placeBottomRight)
        .property("BottomRightMargin", &MiniMapManager::bottomRightMargin)
        .property("ShiftLeftByMapCount", &MiniMapManager::shiftLeftByMapCount)
        .property("ShowCompleteOnFull", &MiniMapManager::showCompleteOnFull)
        .property("PieceToZoneIndex0", &MiniMapManager::pieceToZoneIndex0)
        .property("PieceToZoneIndex1", &MiniMapManager::pieceToZoneIndex1)
        .property("PieceToZoneIndex2", &MiniMapManager::pieceToZoneIndex2)
        .property("PieceToZoneIndex3", &MiniMapManager::pieceToZoneIndex3)
        .property("PieceToZoneIndex4", &MiniMapManager::pieceToZoneIndex4);
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

static RectTransform* GetRectOrLog(const std::shared_ptr<Scene>& scene, const std::string& name, const char* label)
{
    if (!scene)
    {
        return nullptr;
    }
    if (auto obj = scene->GetGameObjectByName(name))
    {
        auto rect = obj->GetComponent<RectTransform>();
        return rect;
    }
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
        return nullptr;
    }
    if (auto obj = scene->GetGameObjectByName(name))
    {
        auto image = obj->GetComponent<Image>();
        return image;
    }
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
static constexpr float kCompleteZOffset = 1.0f;

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

static void CaptureFixedPingPositions(RectTransform* rects[6], Vector3 outPos[6], bool& hasPos)
{
    for (int i = 0; i < 6; ++i)
    {
        if (rects[i])
        {
            outPos[i] = rects[i]->GetPos();
        }
    }
    hasPos = true;
}

static std::string TrimCopy(const std::string& s)
{
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) ++start;
    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(start, end - start);
}

static std::vector<std::string> ParseIdList(const std::string& csv)
{
    std::vector<std::string> out;
    std::stringstream ss(csv);
    std::string token;
    while (std::getline(ss, token, ','))
    {
        const std::string t = TrimCopy(token);
        if (!t.empty())
        {
            out.push_back(t);
        }
    }
    return out;
}

static std::vector<int> ParseIndexList(const std::string& csv)
{
    std::vector<int> out;
    std::stringstream ss(csv);
    std::string token;
    while (std::getline(ss, token, ','))
    {
        const std::string t = TrimCopy(token);
        if (t.empty()) continue;
        try
        {
            int v = std::stoi(t);
            // allow 1..6 (user-facing) or 0..5 (internal)
            if (v >= 1 && v <= 6)
            {
                out.push_back(v - 1);
            }
            else if (v >= 0 && v <= 5)
            {
                out.push_back(v);
            }
        }
        catch (...)
        {
        }
    }
    return out;
}

static bool ContainsId(const std::vector<std::string>& list, const std::string& id)
{
    for (const auto& v : list)
    {
        if (v == id) return true;
    }
    return false;
}

void MiniMapManager::OnInitialize()
{
}

void MiniMapManager::OnStart()
{
    // Source of truth in code (independent from scene JSON values).
    zone0IngredientIds = "Ingredient_Apple";
    zone1IngredientIds = "Ingredient_Donggeurangttaeng";
    zone2IngredientIds = "Ingredient_Pear,Ingredient_Sanjeok";
    zone3IngredientIds = "Ingredient_Batter";
    zone4IngredientIds = "Ingredient_Tofu";

    // Ping index mapping (0-based):
    // ping1 Apple, ping2 Donggeurangttaeng, ping3 Pear, ping4 Sanjeok, ping5 Batter, ping6 Tofu
    zone0PingIndices = "0";
    zone1PingIndices = "1";
    zone2PingIndices = "2,3";
    zone3PingIndices = "4";
    zone4PingIndices = "5";

    auto scene = SceneSystem::Instance().GetCurrentScene();
    if (!scene)
    {
        return;
    }

    if (auto player = scene->GetGameObjectByName(playerObjectName))
    {
        m_Map = player->GetComponent<MiniMapTestScript>();
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
        if (m_ItemPingImages[i])
        {
            m_ItemPingImages[i]->ChangeData("..\\Assets\\Resource\\MiniMap\\map_ghost.png");
        }
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
    if (m_PieceImages[5] && !shrineMapPath.empty())
    {
        m_PieceImages[5]->ChangeData(shrineMapPath);
    }

    m_TreasureImage = GetImage(scene, "UI_MiniMap_Treasure");
    m_TreasureRect = GetRectOrLog(scene, "UI_MiniMap_Treasure", "RectTransform on treasure object");

    if (m_MainPingImage)
    {
        m_MainPingImage->ChangeData("..\\Assets\\Resource\\MiniMap\\map_charater.png");
    }
    if (m_TreasureImage)
    {
        m_TreasureImage->ChangeData("..\\Assets\\Resource\\MiniMap\\map_ghost.png");
    }

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
        if (!shrineMapPath.empty())
        {
            m_CompleteImage->ChangeData(shrineMapPath);
        }
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
    if (useFixedItemPingPositions && !m_HasFixedItemPingPos)
    {
        // Hard-coded fixed positions (UI space)
        m_ItemPingFixedPos[0] = Vector3(1611.500f, 759.400f, 0.0f);
        m_ItemPingFixedPos[1] = Vector3(1812.700f, 752.600f, 0.0f);
        m_ItemPingFixedPos[2] = Vector3(1755.400f, 821.000f, 0.0f);
        m_ItemPingFixedPos[3] = Vector3(1661.700f, 853.400f, 0.0f);
        m_ItemPingFixedPos[4] = Vector3(1791.200f, 876.000f, 0.0f);
        m_ItemPingFixedPos[5] = Vector3(1646.700f, 964.200f, 0.0f);
        m_HasFixedItemPingPos = true;
    }

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
        SetImageActive(m_ItemPingImages[i], m_ItemActive[i]);
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
        if (useFixedItemPingPositions && !m_HasFixedItemPingPos)
        {
            CaptureFixedPingPositions(m_ItemPingRects, m_ItemPingFixedPos, m_HasFixedItemPingPos);
        }
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
    }

    // Shrine map is controlled as Piece_06 to keep the same behavior as other map pieces.
    if (m_CompleteImage)
    {
        SetRectActive(m_CompleteRect, false);
        SetImageActive(m_CompleteImage, false);
        SetImageAlpha(m_CompleteImage, 0.0f);
    }
    if (m_CompleteObject)
    {
        m_CompleteObject->SetActive(false);
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
    Vector3 playerWorld{};
    if (m_Map)
    {
        if (auto owner = m_Map->GetOwner())
        {
            if (auto playerTransform = owner->GetTransform())
            {
                playerWorld = playerTransform->GetWorldPosition();
            }
        }
    }
    else if (m_Player)
    {
        if (auto playerTransform = m_Player->GetComponent<Transform>())
        {
            playerWorld = playerTransform->GetWorldPosition();
        }
    }

    for (int i = 0; i < 6; ++i)
    {
        if (m_ItemSearchObjects[i])
        {
            auto* search = m_ItemSearchObjects[i];
            bool shouldActive = search->hasItem && search->itemType == ItemType::Ingredient && !search->isSearched;
            if (!shouldActive)
            {
                if (m_ItemActive[i])
                {
                    TriggerItemPingActive(i, false);
                }
                if (search->isSearched)
                {
                    m_ItemSearchObjects[i] = nullptr;
                }
                continue;
            }

            if (auto* owner = search->GetOwner())
            {
                if (auto* tr = owner->GetTransform())
                {
                    m_ItemWorldPos[i] = tr->GetWorldPosition();
                }
            }

            if (!m_ItemActive[i])
            {
                TriggerItemPing(i, m_ItemWorldPos[i]);
            }
        }

        if (useFixedItemPingPositions && m_HasFixedItemPingPos && m_ItemPingRects[i])
        {
            // keep fixed positions even when inactive
            m_ItemPingRects[i]->SetPos(m_ItemPingFixedPos[i]);
        }

        if (!m_ItemPingRects[i] || !m_ItemActive[i]) continue;

        if (!useFixedItemPingPositions || !m_HasFixedItemPingPos)
        {
            const Vector2 local = WorldToMiniMap(m_ItemWorldPos[i], worldMin, worldMax, mapSize);
            const Vector3 pingPos(basePos.x + local.x, basePos.y + local.y, basePos.z + kPingZOffset);
            m_ItemPingRects[i]->SetPos(pingPos);
        }
    }

    if (m_GaugeRect && m_HasGaugeBaseSize)
    {
        const float progress = std::clamp(m_Map->GetProgress01(), 0.0f, 1.0f);
        Vector2 size = m_GaugeBaseSize;
        size.x = m_GaugeBaseSize.x * progress;
        m_GaugeRect->SetSize(size);
    }

    // Keep ping on top even when base map refreshes
    BumpPingRenderOrder();
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
    SetImageActive(m_ItemPingImages[i], m_ItemActive[i]);
}

void MiniMapManager::TriggerItemPingActive(int index, bool active)
{
    const int i = ClampItemIndex(index);
    m_ItemActive[i] = active;
    SetRectActive(m_ItemPingRects[i], active);
    SetImageActive(m_ItemPingImages[i], active);
}

void MiniMapManager::TriggerAllItemPingsInactive()
{
    for (int i = 0; i < 6; ++i)
    {
        m_ItemActive[i] = false;
        SetRectActive(m_ItemPingRects[i], m_ItemActive[i]);
        SetImageActive(m_ItemPingImages[i], m_ItemActive[i]);
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
    cout << "Test" << endl;
    TriggerPieceActive(index, true);
    // Runtime piece index order is swapped at 0/1 against the intended visual labels.
    static constexpr int kPieceToZone[5] = { 1, 0, 2, 3, 4 };
    int zoneIndex = index;
    if (index >= 0 && index < 5)
    {
        zoneIndex = kPieceToZone[index];
    }

    ActivateIngredientsForZone(zoneIndex);
    if (m_Map)
    {
        m_Map->TriggerPieceCollected();
    }
}

void MiniMapManager::TriggerShrineEvent(bool active)
{
    m_ShrineEventActive = active;

    // Control shrine as Piece_06 only.
    TriggerPieceActive(5, active);

    if (active)
    {
        BumpRenderOrder(m_PieceImages[5]);
        BumpPingRenderOrder();
    }
}

void MiniMapManager::ActivateIngredientsForZone(int zoneIndex)
{
    if (zoneIndex < 0 || zoneIndex > 4)
    {
        return;
    }
    if (m_ZoneActivated[zoneIndex])
    {
        return;
    }

    const std::string* zoneCsv = nullptr;
    const std::string* zonePingCsv = nullptr;
    switch (zoneIndex)
    {
    case 0: zoneCsv = &zone0IngredientIds; zonePingCsv = &zone0PingIndices; break;
    case 1: zoneCsv = &zone1IngredientIds; zonePingCsv = &zone1PingIndices; break;
    case 2: zoneCsv = &zone2IngredientIds; zonePingCsv = &zone2PingIndices; break;
    case 3: zoneCsv = &zone3IngredientIds; zonePingCsv = &zone3PingIndices; break;
    case 4: zoneCsv = &zone4IngredientIds; zonePingCsv = &zone4PingIndices; break;
    default: return;
    }

    const std::vector<int> indices = (zonePingCsv && !zonePingCsv->empty())
        ? ParseIndexList(*zonePingCsv)
        : std::vector<int>{};

    if (!zoneCsv || zoneCsv->empty())
    {
        if (!indices.empty())
        {
            for (int idx : indices)
            {
                if (idx < 0 || idx > 5) continue;
                m_ItemActive[idx] = true;
                SetRectActive(m_ItemPingRects[idx], true);
            }
            m_ZoneActivated[zoneIndex] = true;
        }
        return;
    }

    auto scene = SceneSystem::Instance().GetCurrentScene();
    if (!scene)
    {
        return;
    }

    const std::vector<std::string> ids = ParseIdList(*zoneCsv);
    if (ids.empty())
    {
        return;
    }

    int pingIndex = 0;
    scene->ForEachGameObject([&](GameObject* obj)
    {
        if (!obj) return;

        auto* search = obj->GetComponent<SearchObject>();
        if (!search) return;
        if (!search->hasItem) return;
        if (search->itemType != ItemType::Ingredient) return;
        if (!ContainsId(ids, search->itemID)) return;
        if (search->isSearched) return;

        int targetIndex = -1;
        if (!indices.empty())
        {
            if (pingIndex >= static_cast<int>(indices.size())) return;
            targetIndex = indices[pingIndex];
            pingIndex++;
        }
        else
        {
            while (pingIndex <= 5 && m_ItemSearchObjects[pingIndex] != nullptr)
            {
                pingIndex++;
            }
            if (pingIndex > 5) return;
            targetIndex = pingIndex;
            pingIndex++;
        }

        if (targetIndex < 0 || targetIndex > 5) return;
        if (m_ItemSearchObjects[targetIndex] != nullptr) return;

        auto* tr = obj->GetTransform();
        if (!tr) return;

        const Vector3 pos = tr->GetWorldPosition();
        m_ItemSearchObjects[targetIndex] = search;
        m_ItemWorldPos[targetIndex] = pos;
        TriggerItemPing(targetIndex, pos);
    });

    m_ZoneActivated[zoneIndex] = true;
}


