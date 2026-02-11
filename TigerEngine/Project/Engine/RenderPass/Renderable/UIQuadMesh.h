#pragma once
#include "pch.h"
#include "UIQuadVertex.h"`

/*
    [ UI Quad Mesh ]

    UI 렌더링을 위한 사각형 메쉬이며 
    해당 메쉬는 Text, Image를 그립니다.

    @date 26.02.02 이성호
*/
class UIQuadMesh
{
private:
    ComPtr<ID3D11Device> device{};

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
    void CreateTextBuffers(uint32_t maxGlyphsInit = 256);
    void EnsureTextCapacity(uint32_t glyphCountNeeded); // 현재 확보한 Glyphs 값을 넘어가면 2배 키우는 함수
    void UploadTextVB(const ComPtr<ID3D11DeviceContext>& ctx, const std::vector<UIQuadVertex>& verts);

    ComPtr<ID3D11Buffer> textVB;
    ComPtr<ID3D11Buffer> textIB;

    uint32_t maxGlyphs = 256;
};