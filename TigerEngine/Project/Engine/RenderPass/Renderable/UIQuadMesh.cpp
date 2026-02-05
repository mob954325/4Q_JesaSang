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
