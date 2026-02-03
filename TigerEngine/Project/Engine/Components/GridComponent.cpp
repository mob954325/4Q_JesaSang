#include "GridComponent.h"
#include "../EngineSystem/GridSystem.h"
#include "../EngineSystem/PhysicsSystem.h"
#include "../Components/PhysicsComponent.h"
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


void DebugPrintBlock(
    const std::string& obj,
    int gx, int gy,
    int cx, int cy,
    const Vector3& worldPos,
    ColliderType type)
{
    std::cout
        << "[" << obj << "]  "
        << "Grid(" << gx << "," << gy << ")  "
        << "Center(" << cx << "," << cy << ")  "
        << "World(" << worldPos.x << "," << worldPos.z << ")  "
        << "Type(" << (int)type << ")\n";
}




void GridComponent::OnInitialize()
{
    GridSystem::Instance().Register(this);

    // 셀 배열 초기화 
    ResizeGrid(width, height);

    // 임의로 (-1,2) 그리드를 걸을 수 없게 설정
    //SetWalkableFromCenter(-1, 2, false);
    //SetWalkableFromCenter(3, -4, false);
}

void GridComponent::OnStart()
{
    // Physics 기반 자동 차단
    BuildBlockedFromPhysics();
}

void GridComponent::OnDestory()
{
    GridSystem::Instance().UnRegister(this);
}

void GridComponent::BuildBlockedFromPhysics()
{
    for (auto& c : cells)
        c.walkable = true;

    auto& map = PhysicsSystem::Instance().m_ActorMap;

    std::cout << "\n==== Physics → Grid Mapping (AABB based, Center Origin) ====\n";

    int centerX = (width - 1) / 2;
    int centerY = (height - 1) / 2;

    for (auto& pair : map)
    {
        PhysicsComponent* phys = pair.first;
        if (!phys || !phys->m_Actor) continue;
        if (phys->IsTrigger()) continue;
        if (phys->GetLayer() & CollisionLayer::Ground) continue;

        auto owner = phys->GetOwner();
        std::string objName = owner ? owner->GetName() : "Unknown";

        Transform* tr = phys->transform;
        if (!tr) continue;

        // Vector3 pos = tr->GetWorldPosition();
        Vector3 pos = tr->GetLocalPosition();

        // -----------------------
        // 1. 월드 AABB
        // -----------------------
        Vector3 minW, maxW;

        if (phys->m_ColliderType == ColliderType::Box)
        {
            PxBounds3 b = phys->m_Actor->getWorldBounds();

            std::cout << "\n[" << objName << "] PxBounds\n";
            std::cout << "  min = (" << b.minimum.x * 100 << ", " << b.minimum.y * 100 << ", " << b.minimum.z * 100 << ")\n";
            std::cout << "  max = (" << b.maximum.x * 100 << ", " << b.maximum.y * 100 << ", " << b.maximum.z * 100 << ")\n";

            std::cout << "  TransformPos = (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";

            minW = { b.minimum.x * 100, 0, b.minimum.z * 100 };
            maxW = { b.maximum.x * 100, 0, b.maximum.z * 100 };
        }
        else
        {
            float r = 0.0f;
            switch (phys->m_ColliderType)
            {
            case ColliderType::Sphere:  r = phys->m_Radius; break;
            case ColliderType::Capsule: r = phys->m_Radius; break;
            default: r = phys->m_HalfExtents.x; break;
            }

            minW = { pos.x - r, 0, pos.z - r };
            maxW = { pos.x + r, 0, pos.z + r };
        }

        // -----------------------
        // 2. AABB → 중앙 기준 Grid
        // -----------------------
        int minCX, minCY;
        int maxCX, maxCY;

        if (!WorldToGridFromCenter(minW, minCX, minCY)) continue;
        if (!WorldToGridFromCenter(maxW, maxCX, maxCY)) continue;

        std::cout << "\n[" << objName << "] "
            << "CenterGridRange: (" << minCX << "," << minCY
            << ") ~ (" << maxCX << "," << maxCY << ")\n";

        // -----------------------
        // 3. 중앙 기준 → 내부 인덱스 변환
        // -----------------------
        int minX = centerX + minCX;
        int minY = centerY + minCY;
        int maxX = centerX + maxCX;
        int maxY = centerY + maxCY;

        for (int y = minY; y <= maxY; ++y)
        {
            for (int x = minX; x <= maxX; ++x)
            {
                if (auto* cell = GetCell(x, y))
                {
                    cell->walkable = false;

                    int cx = x - centerX;
                    int cy = y - centerY;
                    Vector3 cellWorld = GridToWorld(x, y);

                    DebugPrintBlock(
                        objName,
                        x, y,
                        cx, cy,
                        cellWorld,
                        phys->m_ColliderType);
                }
            }
        }
    }

    std::cout << "===========================================\n";
}


//void GridComponent::BuildBlockedFromPhysics()
//{
//    // 전부 Walkable로 리셋
//    for (auto& c : cells)
//        c.walkable = true;
//
//    auto& map = PhysicsSystem::Instance().m_ActorMap;
//
//    for (auto& pair : map)
//    {
//        PhysicsComponent* phys = pair.first;
//        if (!phys || !phys->m_Actor) continue;
//
//        // Trigger Collider 제외
//        if (phys->IsTrigger()) continue;
//
//        // Ground Layer 제외 
//        if (phys->GetLayer() & CollisionLayer::Ground) continue;
//
//        Transform* tr = phys->transform;  //Transform* tr = phys->GetOwner()->GetTransform();
//        if (!tr) continue;
//
//        Vector3 pos = tr->GetWorldPosition();
//
//        int gx, gy;
//        if (!WorldToGrid(pos, gx, gy))
//            continue;
//
//        // Collider 크기만큼 확장
//        float worldRadius = 0.0f;
//        switch (phys->m_ColliderType)
//        {
//        case ColliderType::Box:     worldRadius = phys->m_HalfExtents.x; break;
//        case ColliderType::Sphere:  worldRadius = phys->m_Radius; break;
//        case ColliderType::Capsule: worldRadius = phys->m_Radius; break;
//        default:                    worldRadius = phys->m_HalfExtents.x; break;
//        }
//
//        int radius = (int)(worldRadius / cellSize); // +1;
//        for (int y = -radius; y <= radius; ++y)
//        {
//            for (int x = -radius; x <= radius; ++x)
//            {
//                if (auto* cell = /*GetCell*/GetCellFromCenter(gx + x, gy + y))
//                    cell->walkable = false;
//            }
//        }
//    }
//
//    std::cout << "==== Blocked Grid Cells ====\n";
//
//    for (auto& c : cells)
//    {
//        if (!c.walkable)
//        {
//            // 내부 인덱스 → 중앙 기준 좌표로 변환
//            int cx = c.x - (width / 2);
//            int cy = c.y - (height / 2);
//
//            std::cout << "Blocked: ("
//                << cx << ", "
//                << cy << ")\n";
//        }
//    }
//
//    std::cout << "============================\n";
//}


// 수동 
//void GridComponent::BuildBlockedFromPhysics()
//{
//    for (auto& c : cells)
//        c.walkable = true;
//
//    auto& map = PhysicsSystem::Instance().m_ActorMap;
//
//    std::cout << "\n==== Physics → Grid Mapping Debug ====\n";
//
//    int centerX = (width - 1) / 2;
//    int centerY = (height - 1) / 2;
//
//    for (auto& pair : map)
//    {
//        PhysicsComponent* phys = pair.first;
//        if (!phys || !phys->m_Actor) continue;
//        if (phys->IsTrigger()) continue;
//        if (phys->GetLayer() & CollisionLayer::Ground) continue;
//
//        auto owner = phys->GetOwner();
//        std::string objName = owner ? owner->GetName() : "Unknown";
//
//        Transform* tr = phys->transform;
//        if (!tr) continue;
//
//        Vector3 pos = tr->GetWorldPosition();
//
//        int gx, gy;
//        if (!WorldToGrid(pos, gx, gy))
//        {
//            std::cout << "[OUT OF GRID] " << objName
//                << " world(" << pos.x << "," << pos.z << ")\n";
//            continue;
//        }
//
//        float worldRadius = 0.0f;
//        switch (phys->m_ColliderType)
//        {
//        case ColliderType::Box:     worldRadius = phys->m_HalfExtents.x; break;
//        case ColliderType::Sphere:  worldRadius = phys->m_Radius; break;
//        case ColliderType::Capsule: worldRadius = phys->m_Radius; break;
//        default:                    worldRadius = phys->m_HalfExtents.x; break;
//        }
//
//        /*int radius = (int)ceil(worldRadius / cellSize);
//
//        std::cout << "\n[" << objName << "] base Grid("
//            << gx << "," << gy << ") radius=" << radius
//            << " world(" << pos.x << "," << pos.z << ")\n";
//
//        for (int y = -radius; y <= radius; ++y)
//        {
//            for (int x = -radius; x <= radius; ++x)
//            {
//                int ix = gx + x;
//                int iy = gy + y;
//
//                if (auto* cell = GetCell(ix, iy))
//                {
//                    cell->walkable = false;
//
//                    int cx = ix - centerX;
//                    int cy = iy - centerY;
//
//                    Vector3 cellWorld = GridToWorld(ix, iy);
//
//                    DebugPrintBlock(
//                        objName,
//                        ix, iy,
//                        cx, cy,
//                        cellWorld,
//                        phys->m_ColliderType);
//                }
//            }
//        }*/
//
//        if (auto* cell = GetCell(gx, gy))
//        {
//            cell->walkable = false;
//
//            int cx = gx - centerX;
//            int cy = gy - centerY;
//            Vector3 cellWorld = GridToWorld(gx, gy);
//
//            DebugPrintBlock(
//                objName,
//                gx, gy,
//                cx, cy,
//                cellWorld,
//                phys->m_ColliderType);
//        }
//    }
//
//    std::cout << "=====================================\n";
//}


// Width 와 Height에 의해 재설정되는 그리드 cell 
void GridComponent::ResizeGrid(int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;

    cells.clear();
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
    int centerX = (width - 1) / 2;
    int centerY = (height - 1) / 2;

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
    // Vector3 origin = t->GetWorldPosition();
    Vector3 origin = t->GetLocalPosition();

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
    Vector3 origin = t->GetLocalPosition(); // 그리드 중심

    float offsetX = (width * 0.5f - 0.5f) * cellSize;
    float offsetZ = (height * 0.5f - 0.5f) * cellSize;

    float localX = pos.x - origin.x + offsetX;
    float localZ = pos.z - origin.z + offsetZ;

    outX = (int)floor(localX / cellSize);
    outY = (int)floor(localZ / cellSize);

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

    //auto t = GetOwner()->GetTransform();
    //Vector3 origin = t->GetWorldPosition();

    //return {
    //    origin.x + (cx + 0.5f) * cellSize,
    //    origin.y,
    //    origin.z + (cy + 0.5f) * cellSize
    //};
}

bool GridComponent::WorldToGridFromCenter(const Vector3& pos, int& outCX, int& outCY)
{
    auto t = GetOwner()->GetTransform();
    Vector3 origin = t->GetWorldPosition(); // 그리드 중앙

    float localX = pos.x - origin.x;
    float localZ = pos.z - origin.z;

    outCX = (int)floor(localX / cellSize + 0.5f);
    outCY = (int)floor(localZ / cellSize + 0.5f);

    int centerX = (width - 1) / 2;
    int centerY = (height - 1) / 2;

    int ix = centerX + outCX;
    int iy = centerY + outCY;

    return !(ix < 0 || iy < 0 || ix >= width || iy >= height);
}
