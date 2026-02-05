#include "GridSystem.h"
#include "../Components/GridComponent.h"

void GridSystem::Register(GridComponent* g)
{
    grids.push_back(g);
}

void GridSystem::UnRegister(GridComponent* g)
{
    grids.erase(std::remove(grids.begin(), grids.end(), g), grids.end());
}
