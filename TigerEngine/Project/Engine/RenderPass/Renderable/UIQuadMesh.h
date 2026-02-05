#pragma once
#include "pch.h"


struct UIQuadVertex // vs
{
    Vector3 pos;
    Vector2 uv;

    UIQuadVertex(float x, float y, float z, float u, float v) : pos(x, y, z), uv(u, v) {}
    UIQuadVertex(Vector3 p, Vector2 u) : pos(p), uv(u) {}
};

/*
    [ UI Quad Mesh ]

    UI 렌더링을 위한 사각형 메쉬이며 
    해당 메쉬는 Text, Image를 그립니다.

    @date 26.02.02 이성호
*/
class UIQuadMesh
{
private:
    // VB, IB
    ComPtr<ID3D11Buffer> vertexBuffer{};
    ComPtr<ID3D11Buffer> indexBuffer{};
    UINT indexCount = 6;
    UINT stride = sizeof(UIQuadVertex);
    UINT offset{};

public:
    void Create(const ComPtr<ID3D11Device>& device);
    
    const ComPtr<ID3D11Buffer>& GetVertexBuffer();
    const ComPtr<ID3D11Buffer>& GetIndexBuffer();

    UINT GetIndexCount() const;
    UINT GetStride() const;
    UINT GetOffset() const;

    // ===== Text =====
    std::vector<UIQuadVertex> cpuVerts; // 복사할 정점 데이터
    std::vector<uint16_t> cpuInds;      // 인덱스 데이터

    uint32_t maxGlyphs = 256;
};