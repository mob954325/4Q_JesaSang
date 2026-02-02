#pragma once
#include "../../../Base/pch.h"

struct TextureResource
{
    ComPtr<ID3D11ShaderResourceView> srv;
    ComPtr<ID3D11Texture2D> tex;
    Vector2 texSizePx; // 가로 세로 텍스처 크기 (px)
};