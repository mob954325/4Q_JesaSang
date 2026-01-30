#include "MiniMapTestScript.h"

#include "Components/Transform.h"
#include "Object/GameObject.h"
#include "System/InputSystem.h"
#include "Util/ComponentAutoRegister.h"
#include "Util/JsonHelper.h"

#include <algorithm>
#include <iostream>

REGISTER_COMPONENT(MiniMapTestScript);

RTTR_REGISTRATION
{
    rttr::registration::class_<MiniMapTestScript>("MiniMapTestScript")
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr)
        .property("WorldMin", &MiniMapTestScript::worldMin)
        .property("WorldMax", &MiniMapTestScript::worldMax)
        .property("MapSize", &MiniMapTestScript::mapSize)
        .property("TotalPieces", &MiniMapTestScript::totalPieces)
        .property("CollectedPieces", &MiniMapTestScript::collectedPieces);
}

static Vector2 WorldToMiniMap(const Vector3& worldPos, const Vector3& worldMin, const Vector3& worldMax, const Vector2& mapSize)
{
    const float dx = worldMax.x - worldMin.x;
    const float dz = worldMax.z - worldMin.z;
    const float nx = (dx != 0.0f) ? (worldPos.x - worldMin.x) / dx : 0.0f;
    const float nz = (dz != 0.0f) ? (worldPos.z - worldMin.z) / dz : 0.0f;

    Vector2 out;
    out.x = std::clamp(nx, 0.0f, 1.0f) * mapSize.x;
    out.y = std::clamp(nz, 0.0f, 1.0f) * mapSize.y;
    return out;
}

void MiniMapTestScript::OnInitialize()
{
    std::cout << "[MiniMapTest] OnInitialize" << std::endl;
}

void MiniMapTestScript::OnStart()
{
    std::cout << "[MiniMapTest] OnStart" << std::endl;
}

void MiniMapTestScript::OnUpdate(float delta)
{
    (void)delta;

    Transform* transform = nullptr;
    if (auto owner = GetOwner())
    {
        transform = owner->GetTransform();
    }
    if (!transform)
    {
        return;
    }

    if (Input::GetKeyDown(DirectX::Keyboard::Keys::M))
    {
        const Vector3 pos = transform->GetWorldPosition();
        const Vector2 mm = WorldToMiniMap(pos, worldMin, worldMax, mapSize);
        std::cout << "[MiniMapTest] world("
            << pos.x << ", " << pos.y << ", " << pos.z
            << ") -> map(" << mm.x << ", " << mm.y << ")" << std::endl;
    }

    if (Input::GetKeyDown(DirectX::Keyboard::Keys::O))
    {
        collectedPieces = std::min(collectedPieces + 1, totalPieces);
        std::cout << "[MiniMapTest] pieces " << collectedPieces << " / " << totalPieces << std::endl;
    }
}

nlohmann::json MiniMapTestScript::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void MiniMapTestScript::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}
