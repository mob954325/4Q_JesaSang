#pragma once

#include "Components/ScriptComponent.h"

class RectTransform;
class Image;
class MiniMapTestScript;
class GameObject;
class SearchObject;

class MiniMapManager : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    MiniMapManager() { SetName("MiniMapManager"); }
    ~MiniMapManager() override = default;

    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    std::string playerObjectName = "Player";
    std::string baseMapObjectName = "UI_MiniMap_Base";
    std::string completeMapObjectName = "UI_MiniMap_Complete";
    std::string mainPingObjectName = "UI_MiniMap_MainPing";
    std::string gaugeObjectName = "UI_MiniMap_GaugeFill";
    std::string gaugeBgObjectName = "UI_MiniMap_GaugeBG";
    std::string baseMapPath = "..\\Assets\\Resource\\MiniMap\\MAP_GR.png";
    std::string shrineMapPath = "..\\Assets\\Resource\\MiniMap\\MAP_S.png";
    std::string completeMapPath = "..\\Assets\\Resource\\MiniMap\\Map.png";

    std::string itemPingObjectName1 = "UI_MiniMap_ItemPing_01";
    std::string itemPingObjectName2 = "UI_MiniMap_ItemPing_02";
    std::string itemPingObjectName3 = "UI_MiniMap_ItemPing_03";
    std::string itemPingObjectName4 = "UI_MiniMap_ItemPing_04";
    std::string itemPingObjectName5 = "UI_MiniMap_ItemPing_05";
    std::string itemPingObjectName6 = "UI_MiniMap_ItemPing_06";

    std::string pieceObjectName1 = "UI_MiniMap_Piece_01";
    std::string pieceObjectName2 = "UI_MiniMap_Piece_02";
    std::string pieceObjectName3 = "UI_MiniMap_Piece_03";
    std::string pieceObjectName4 = "UI_MiniMap_Piece_04";
    std::string pieceObjectName5 = "UI_MiniMap_Piece_05";
    std::string pieceObjectName6 = "UI_MiniMap_Piece_06";

    // Map piece -> ingredient IDs (comma-separated, e.g., "Ingredient_Apple,Ingredient_Pear")
    std::string zone0IngredientIds = "";
    std::string zone1IngredientIds = "";
    std::string zone2IngredientIds = "";
    std::string zone3IngredientIds = "";
    std::string zone4IngredientIds = "";
    // Map piece -> ping indices (comma-separated, e.g., "0,2,5")
    std::string zone0PingIndices = "";
    std::string zone1PingIndices = "";
    std::string zone2PingIndices = "";
    std::string zone3PingIndices = "";
    std::string zone4PingIndices = "";

    Vector3 itemWorldPos1{ -300.0f, 0.0f, -300.0f };
    Vector3 itemWorldPos2{ 300.0f, 0.0f, -300.0f };
    Vector3 itemWorldPos3{ -300.0f, 0.0f, 300.0f };
    Vector3 itemWorldPos4{ 300.0f, 0.0f, 300.0f };
    Vector3 itemWorldPos5{ 0.0f, 0.0f, 0.0f };
    Vector3 itemWorldPos6{ 200.0f, 0.0f, 100.0f };
    bool debugShowAllItemPings = false;
    bool debugShowAllPieces = true;
    bool useFixedItemPingPositions = true;
    bool placeBottomRight = true;
    Vector2 bottomRightMargin{ 20.0f, 20.0f };
    int shiftLeftByMapCount = 1;
    bool showCompleteOnFull = false;

    // External trigger hooks (call from gameplay code on pickup)
    void TriggerItemPing(int index, const Vector3& worldPos);
    void TriggerItemPingActive(int index, bool active);
    void TriggerAllItemPingsInactive();
    void TriggerPieceActive(int index, bool active);
    void TriggerAllPiecesInactive();
    void TriggerItemCollected(int index, const Vector3& worldPos);
    void TriggerPieceCollected(int index);
    void TriggerShrineEvent(bool active);

private:
    MiniMapTestScript* m_Map = nullptr;
    GameObject* m_Player = nullptr;
    GameObject* m_BaseObject = nullptr;
    GameObject* m_CompleteObject = nullptr;
    RectTransform* m_BaseRect = nullptr;
    RectTransform* m_CompleteRect = nullptr;
    RectTransform* m_MainPingRect = nullptr;
    RectTransform* m_GaugeRect = nullptr;
    RectTransform* m_GaugeBgRect = nullptr;
    RectTransform* m_ItemPingRects[6]{};
    RectTransform* m_PieceRects[6]{};
    Image* m_PieceImages[6]{};
    Image* m_BaseImage = nullptr;
    Image* m_CompleteImage = nullptr;
    Image* m_MainPingImage = nullptr;
    Image* m_ItemPingImages[6]{};
    Image* m_TreasureImage = nullptr;
    RectTransform* m_TreasureRect = nullptr;
    bool m_UsingCompleteMap = false;
    bool m_ShrineEventActive = false;
    bool m_ZoneActivated[5]{};

    Vector2 m_GaugeBaseSize{};
    bool m_HasGaugeBaseSize = false;

    Vector3 m_ItemWorldPos[6]{};
    bool m_ItemActive[6]{};
    SearchObject* m_ItemSearchObjects[6]{};
    Vector3 m_ItemPingFixedPos[6]{};
    bool m_HasFixedItemPingPos = false;
    bool m_PieceActive[6]{};
    bool m_LayoutApplied = false;

    void BumpPingRenderOrder();
    void ApplyLayout();
    void ActivateIngredientsForZone(int zoneIndex);
};
