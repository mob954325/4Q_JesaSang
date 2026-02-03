#pragma once
#include "Components/ScriptComponent.h"

class MiniMapTestScript : public ScriptComponent
{
    RTTR_ENABLE(ScriptComponent)
public:
    MiniMapTestScript() { SetName("MiniMapTestScript"); }
    ~MiniMapTestScript() override = default;

    void OnInitialize() override;
    void OnStart() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    Vector3 worldMin{ -500.0f, 0.0f, -500.0f };
    Vector3 worldMax{ 500.0f, 0.0f, 500.0f };
    Vector2 mapSize{ 256.0f, 256.0f };
    int totalPieces = 6;
    int collectedPieces = 0;
    int totalItems = 6;
    int collectedItems = 0;

    Vector2 GetPlayerMapPos() const { return playerMapPos; }
    const Vector3& GetWorldMin() const { return worldMin; }
    const Vector3& GetWorldMax() const { return worldMax; }
    const Vector2& GetMapSize() const { return mapSize; }
    float GetProgress01() const;
    void OnPieceCollected();
    void OnItemCollected();
    // External trigger hooks (call from gameplay code on pickup)
    void TriggerPieceCollected();
    void TriggerItemCollected();

private:
    Vector2 playerMapPos{};
    void HandleDebugInput(float delta);
};
