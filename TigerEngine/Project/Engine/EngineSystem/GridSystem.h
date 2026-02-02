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

    GridComponent* GetMainGrid()
    {
        if (grids.empty()) return nullptr;

        return grids[0];
    }
};
