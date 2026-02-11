#pragma once
#include "../pch.h"

// [ RenderBlendType ]
// RenderComponent의 멤버로 소유됩니다.
// 하이브리드 렌더링 방식의 분기로 사용됩니다.
enum class RenderBlendType
{
    Opaque,
    Transparent
};


// [ Light Type ]
// Light Component의 멤버로 소유됩니다.
enum class LightType
{
    Directional = 0,
    Point = 1,
    Spot = 2
};


// [Decal Type]
// Decal Component의 멤버로 소유됩니다.
enum class DecalType
{
    TextureMap = 0,
    RingEffect = 1
};


// ------------------------------
// Physics Enum (엔진 내부용)
// ------------------------------
enum class PhysicsBodyType
{
    Static,
    Dynamic,
    Kinematic
};

enum class ColliderType
{
    Box,
    Sphere,
    Capsule
};

enum class CollisionLayer : uint32_t
{
    Default = 1 << 0,
    Player = 1 << 1,
    Enemy = 1 << 2,
    World = 1 << 3,
    Trigger = 1 << 4,
    Projectile = 1 << 5,
    Vision = 1 << 6,
    Ground = 1 << 7 // Grid에서 Ground는 탐색영역으로 감지한다. 
};




////////////////////////////////////////////////////////////////////
///////////////                               //////////////////////
///////////////  클라이언트 Script Enum RTTR   //////////////////////
///////////////                               //////////////////////
////////////////////////////////////////////////////////////////////

enum class ItemType
{
    Ingredient, Piece, Food, None
};