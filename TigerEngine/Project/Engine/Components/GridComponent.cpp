#include "GridComponent.h"
#include "../EngineSystem/GridSystem.h"
#include "../Components/Transform.h"
#include "../Object/GameObject.h"
#include "../Util/JsonHelper.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<GridComponent>("GridComponent")
        .constructor<>()
        (rttr::policy::ctor::as_std_shared_ptr)
        .property("Width", &GridComponent::width)
        .property("Height", &GridComponent::height)
        .property("CellSize", &GridComponent::cellSize);
}

nlohmann::json GridComponent::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void GridComponent::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
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

    // 임의로 (1,3) 그리드를 걸을 수 없게 하드코딩
    int centerX = width / 2;
    int centerY = height / 2;

    int relX = centerX + (-1); // -1은 원점 기준 x 좌표
    int relY = centerY + 2;    // 1은 원점 기준 y 좌표

    GridCell* cell = GetCell(relX, relY);
    if (cell)
        cell->walkable = false;
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

    // 전체 그리드 중심을 원점으로 맞추기
    float offsetX = (width * 0.5f - 0.5f) * cellSize;
    float offsetZ = (height * 0.5f - 0.5f) * cellSize;

    return // 그리드 좌표를 중점 기준으로 
    {
        origin.x + (x * cellSize) - offsetX,
        origin.y,
        origin.z + (y * cellSize) - offsetZ
    };
}



bool GridComponent::WorldToGrid(const Vector3& pos, int& outX, int& outY)
{
    auto t = GetOwner()->GetTransform();
    Vector3 origin = t->GetWorldPosition();

    // 원점을 중앙으로 이동
    float offsetX = (width * 0.5f - 0.5f) * cellSize;
    float offsetZ = (height * 0.5f - 0.5f) * cellSize;

    Vector3 local;
    local.x = pos.x - origin.x + offsetX;
    local.z = pos.z - origin.z + offsetZ;

    outX = (int)(local.x / cellSize);
    outY = (int)(local.z / cellSize);

    return !(outX < 0 || outY < 0 || outX >= width || outY >= height);
}


GridCell* GridComponent::GetCellFromCenter(int cx, int cy)
{
    int centerX = width / 2;
    int centerY = height / 2;

    int ix = centerX + cx; // 중앙 기준 → 내부 배열 인덱스
    int iy = centerY + cy;

    return GetCell(ix, iy);
}

bool GridComponent::IsWalkableFromCenter(int cx, int cy)
{
    GridCell* cell = GetCellFromCenter(cx, cy);
    return cell ? cell->walkable : false;
}
