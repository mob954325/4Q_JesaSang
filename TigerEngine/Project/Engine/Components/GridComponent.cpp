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

    // 셀 배열 초기화 
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

    // 임의로 (-1,2) 그리드를 걸을 수 없게 설정
    SetWalkableFromCenter(-1, 2, false);
    SetWalkableFromCenter(3, -4, false);
}

void GridComponent::OnDestory()
{
    GridSystem::Instance().UnRegister(this);
}


//----------------------------------------
// 셀 접근 / 조회
//----------------------------------------

GridCell* GridComponent::GetCell(int x, int y)
{
    if (x < 0 || y < 0 || x >= width || y >= height)
        return nullptr;

    return &cells[y * width + x];
}

bool GridComponent::IsWalkable(int x, int y)
{
    GridCell* cell = GetCell(x, y);
    return cell ? cell->walkable : false;
}

//----------------------------------------
// 중앙 기준 좌표 접근
//----------------------------------------
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

//----------------------------------------
// 중앙 기준 좌표로 Walkable 상태 설정
//----------------------------------------
void GridComponent::SetWalkableFromCenter(int cx, int cy, bool walkable)
{
    GridCell* cell = GetCellFromCenter(cx, cy);
    if (cell)
        cell->walkable = walkable;
}

//----------------------------------------
// Grid <-> World 변환
//----------------------------------------

// 0,0이 좌측 하단 기준
Vector3 GridComponent::GridToWorld(int x, int y)
{
    auto t = GetOwner()->GetTransform();
    Vector3 origin = t->GetWorldPosition();

    float offsetX = (width * 0.5f - 0.5f) * cellSize;
    float offsetZ = (height * 0.5f - 0.5f) * cellSize;

    return {
        origin.x + (x + 0.5f) * cellSize - offsetX, // +0.5f: 셀 중앙 기준
        origin.y,
        origin.z + (y + 0.5f) * cellSize - offsetZ
    };
}

// World -> Grid
bool GridComponent::WorldToGrid(const Vector3& pos, int& outX, int& outY)
{
    auto t = GetOwner()->GetTransform();
    Vector3 origin = t->GetWorldPosition();

    float offsetX = (width * 0.5f - 0.5f) * cellSize;
    float offsetZ = (height * 0.5f - 0.5f) * cellSize;

    // 셀 중앙 기준으로 보정
    Vector3 local;
    local.x = pos.x - origin.x + offsetX - 0.5f * cellSize;
    local.z = pos.z - origin.z + offsetZ - 0.5f * cellSize;

    outX = static_cast<int>(local.x / cellSize);
    outY = static_cast<int>(local.z / cellSize);

    return !(outX < 0 || outY < 0 || outX >= width || outY >= height);
}

// 중앙 기준 좌표 -> World
Vector3 GridComponent::GridToWorldFromCenter(int cx, int cy)
{
    auto t = GetOwner()->GetTransform();
    Vector3 origin = t->GetWorldPosition(); // 오브젝트 위치 = 그리드 중앙

    // cx, cy: 중앙 기준 좌표 (-center ~ +center)
    float offsetX = cx * cellSize;
    float offsetZ = cy * cellSize;

    return {
        origin.x + offsetX,
        origin.y,
        origin.z + offsetZ
    };
}