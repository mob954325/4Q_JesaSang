#include "UIQuadMesh.h"

void UIQuadMesh::Create(const ComPtr<ID3D11Device>& device)
{
    UIQuadVertex QuadVertices[] = // 좌측 상단을 기준으로한 쿼드 ( 0 - 1 )
    {
        { Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f) }, // LT
        { Vector3(1.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f) }, // RT
        { Vector3(0.0f, 1.0f, 0.0f), Vector2(0.0f, 1.0f) }, // LB
        { Vector3(1.0f, 1.0f, 0.0f), Vector2(1.0f, 1.0f) }, // RB
    };

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(UIQuadVertex) * ARRAYSIZE(QuadVertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = QuadVertices;	// 배열 데이터 할당.
    HR_T(device->CreateBuffer(&vbDesc, &vbData, vertexBuffer.GetAddressOf()));
    stride = sizeof(UIQuadVertex);		// 버텍스 버퍼 정보
    offset = 0;

    // 인덱스 버퍼 생성
    WORD indices[] =
    {
        0,1,2,
        1,3,2
    };
    indexCount = ARRAYSIZE(indices);	// 인덱스 개수 저장.
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices;
    HR_T(device->CreateBuffer(&ibDesc, &ibData, indexBuffer.GetAddressOf()));

    this->device = device;
}

const ComPtr<ID3D11Buffer>& UIQuadMesh::GetVertexBuffer()
{
    return vertexBuffer;
}

const ComPtr<ID3D11Buffer>& UIQuadMesh::GetIndexBuffer()
{
    return indexBuffer;
}

UINT UIQuadMesh::GetIndexCount() const
{
    return indexCount;
}

UINT UIQuadMesh::GetStride() const
{
    return stride;
}

UINT UIQuadMesh::GetOffset() const
{
    return offset;
}


// ===== text =====

void UIQuadMesh::CreateTextBuffers(uint32_t maxGlyphsInit)
{
    maxGlyphs = maxGlyphsInit;

    // VB (dynamic)
    D3D11_BUFFER_DESC vb{};
    vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vb.Usage = D3D11_USAGE_DYNAMIC;
    vb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vb.ByteWidth = sizeof(UIQuadVertex) * 4 * maxGlyphs;
    HR_T(device->CreateBuffer(&vb, nullptr, textVB.GetAddressOf()));

    // IB (immutable)
    std::vector<uint16_t> inds;
    inds.reserve(6 * maxGlyphs);
    for (uint32_t i = 0; i < maxGlyphs; ++i)
    {
        uint16_t base = (uint16_t)(i * 4);
        inds.push_back(base + 0);
        inds.push_back(base + 1);
        inds.push_back(base + 2);
        inds.push_back(base + 1);
        inds.push_back(base + 3);
        inds.push_back(base + 2);
    }

    D3D11_BUFFER_DESC ib{};
    ib.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ib.Usage = D3D11_USAGE_IMMUTABLE;
    ib.ByteWidth = (UINT)(inds.size() * sizeof(uint16_t));

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = inds.data();
    HR_T(device->CreateBuffer(&ib, &init, textIB.GetAddressOf()));
}

void UIQuadMesh::EnsureTextCapacity(uint32_t glyphCountNeeded)
{
    if (glyphCountNeeded <= maxGlyphs) return;

    // 2배씩 확장
    uint32_t newMax = maxGlyphs ? maxGlyphs : 256;
    while (glyphCountNeeded > newMax) newMax *= 2;
    maxGlyphs = newMax;

    // VB 재생성
    D3D11_BUFFER_DESC vb{};
    vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vb.Usage = D3D11_USAGE_DYNAMIC;
    vb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vb.ByteWidth = sizeof(UIQuadVertex) * 4 * maxGlyphs;
    HR_T(device->CreateBuffer(&vb, nullptr, textVB.ReleaseAndGetAddressOf()));

    // IB 재생성
    std::vector<uint16_t> inds;
    inds.reserve(6 * maxGlyphs);
    for (uint32_t i = 0; i < maxGlyphs; ++i)
    {
        uint16_t base = (uint16_t)(i * 4);
        inds.push_back(base + 0);
        inds.push_back(base + 1);
        inds.push_back(base + 2);
        inds.push_back(base + 1);
        inds.push_back(base + 3);
        inds.push_back(base + 2);
    }

    D3D11_BUFFER_DESC ib{};
    ib.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ib.Usage = D3D11_USAGE_IMMUTABLE;
    ib.ByteWidth = (UINT)(inds.size() * sizeof(uint16_t));

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = inds.data();
    HR_T(device->CreateBuffer(&ib, &init, textIB.ReleaseAndGetAddressOf()));
}

void UIQuadMesh::UploadTextVB(const ComPtr<ID3D11DeviceContext>& context,
    const std::vector<UIQuadVertex>& verts)
{
    if (verts.empty()) return;

    D3D11_MAPPED_SUBRESOURCE mapped{};
    HR_T(context->Map(textVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
    memcpy(mapped.pData, verts.data(), verts.size() * sizeof(UIQuadVertex));
    context->Unmap(textVB.Get(), 0);
}