#pragma once
#include "System/Singleton.h"
#include <vector>

class GridComponent;

class GridSystem : public Singleton<GridSystem>
{
    std::vector<GridComponent*> grids;

public:
    GridSystem(token) {}

    void Register(GridComponent* g);
    void UnRegister(GridComponent* g);

    void FixedUpdate(float delta);

    GridComponent* GetMainGrid()
    {
        if (grids.empty()) return nullptr;

        return grids[0];
    }
};
