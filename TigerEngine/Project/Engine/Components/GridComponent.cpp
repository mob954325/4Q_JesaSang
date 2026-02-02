#include "GridComponent.h"
#include "../EngineSystem/GridSystem.h"
#include "../Components/Transform.h"
#include "../Object/GameObject.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<GridComponent>("GridComponent")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("Width", &GridComponent::width)
        .property("Height", &GridComponent::height)
        .property("CellSize", &GridComponent::cellSize);
}


void GridComponent::OnInitialize()
{
    GridSystem::Instance().Register(this);

    cells.resize(width * height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            auto& c = cells[y * width + x];
            c.x = x;
            c.y = y;
            c.walkable = true;
        }
    }
}


void GridComponent::OnDestory()
{
    GridSystem::Instance().UnRegister(this);
}


GridCell* GridComponent::GetCell(int x, int y)
{
    if (x < 0 || y < 0 || x >= width || y >= height)
        return nullptr;

    return &cells[y * width + x];
}


bool GridComponent::IsWalkable(int x, int y) const
{
    if (x < 0 || y < 0 || x >= width || y >= height)
        return false;

    return cells[y * width + x].walkable;
}


Vector3 GridComponent::GridToWorld(int x, int y)
{
    auto t = GetOwner()->GetTransform();
    Vector3 origin = t->GetWorldPosition();

    return 
    {
        origin.x + x * cellSize + cellSize * 0.5f,
        origin.y,
        origin.z + y * cellSize + cellSize * 0.5f
    };
}


bool GridComponent::WorldToGrid(const Vector3& pos, int& outX, int& outY)
{
    auto t = GetOwner()->GetTransform();
    Vector3 origin = t->GetWorldPosition();

    Vector3 local = pos - origin;

    outX = (int)(local.x / cellSize);
    outY = (int)(local.z / cellSize);

    return !(outX < 0 || outY < 0 || outX >= width || outY >= height);
}
