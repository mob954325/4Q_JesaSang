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
        .property("CollectedPieces", &MiniMapTestScript::collectedPieces)
        .property("TotalItems", &MiniMapTestScript::totalItems)
        .property("CollectedItems", &MiniMapTestScript::collectedItems);
}

static Vector2 WorldToMiniMap(const Vector3& worldPos, const Vector3& worldMin, const Vector3& worldMax, const Vector2& mapSize)
{
    const float dx = worldMax.x - worldMin.x;
    const float dz = worldMax.z - worldMin.z;
    const float nx = (dx != 0.0f) ? (worldPos.x - worldMin.x) / dx : 0.0f;
    const float nz = (dz != 0.0f) ? (worldPos.z - worldMin.z) / dz : 0.0f;

    Vector2 out;
    out.x = std::clamp(nx, 0.0f, 1.0f) * mapSize.x;
    // Flip Y so up in world maps to up on minimap.
    out.y = (1.0f - std::clamp(nz, 0.0f, 1.0f)) * mapSize.y;
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

    const Vector3 pos = transform->GetWorldPosition();
    playerMapPos = WorldToMiniMap(pos, worldMin, worldMax, mapSize);

    // Local test hook: trigger map/item collect events by key.
    if (Input::GetKeyDown(DirectX::Keyboard::Keys::M))
    {
        OnPieceCollected();
    }

    if (Input::GetKeyDown(DirectX::Keyboard::Keys::O))
    {
        OnItemCollected();
    }

    if (Input::GetKeyDown(DirectX::Keyboard::Keys::P))
    {
        const char* ownerName = GetOwner() ? GetOwner()->GetName().c_str() : "null";
        std::cout << "[MiniMapTest] owner=" << ownerName << " this=" << this
            << " player map pos(" << playerMapPos.x << ", " << playerMapPos.y
            << "), pieces " << collectedPieces << " / " << totalPieces
            << ", items " << collectedItems << " / " << totalItems << std::endl;
    }
}

float MiniMapTestScript::GetProgress01() const
{
    if (totalPieces <= 0)
    {
        return 0.0f;
    }

    const float progress = static_cast<float>(collectedPieces) / static_cast<float>(totalPieces);
    return std::clamp(progress, 0.0f, 1.0f);
}

void MiniMapTestScript::OnPieceCollected()
{
    collectedPieces = std::min(collectedPieces + 1, totalPieces);
}

void MiniMapTestScript::OnItemCollected()
{
    collectedItems = std::min(collectedItems + 1, totalItems);
}

void MiniMapTestScript::TriggerPieceCollected()
{
    OnPieceCollected();
}

void MiniMapTestScript::TriggerItemCollected()
{
    OnItemCollected();
}

nlohmann::json MiniMapTestScript::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void MiniMapTestScript::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);
}
