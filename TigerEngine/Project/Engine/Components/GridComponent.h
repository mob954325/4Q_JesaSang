#pragma once
#include "pch.h"
#include "../Object/Component.h"

struct GridCell
{
    bool walkable = true;
    int x = 0;
    int y = 0;
};


class GridComponent : public Component
{
    RTTR_ENABLE(Component)
public:
    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

public:
    int width = 50;
    int height = 50;
    float cellSize = 100.0f;

    std::vector<GridCell> cells;

public:
    GridComponent() = default;
    ~GridComponent() = default;

    void OnInitialize() override;
    void OnDestory() override;

    GridCell* GetCell(int x, int y);
    bool IsWalkable(int x, int y) const;

    Vector3 GridToWorld(int x, int y);
    bool WorldToGrid(const Vector3& pos, int& outX, int& outY);
};
