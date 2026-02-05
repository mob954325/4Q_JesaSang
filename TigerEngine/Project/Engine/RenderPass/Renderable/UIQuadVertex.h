#pragma once
#include "../Base/pch.h"

struct UIQuadVertex // vs
{
    Vector3 pos;
    Vector2 uv;

    UIQuadVertex(float x, float y, float z, float u, float v) : pos(x, y, z), uv(u, v) {}
    UIQuadVertex(Vector3 p, Vector2 u) : pos(p), uv(u) {}
};
