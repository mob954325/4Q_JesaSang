#include "UIManager.h"

void UIManager::Init(ComPtr<ID3D11Device>& dev, ComPtr<ID3D11DeviceContext>& ctx)
{
    device = dev;
    context = ctx;
}

void UIManager::SetSize(int w, int h)
{
    width = w;
    height = h;

    proj = XMMatrixOrthographicOffCenterLH(
        0.0f, (float)width,		// left, right
        (float)height, 0.0f,	// bottom, top
        0.0f, 1.0f				// near, far
    );
}

Vector2 UIManager::GetSize()
{
    return { static_cast<float>(width), static_cast<float>(height) };
}

Matrix UIManager::GetProjection() const
{
    return proj;
}

void UIManager::LoadFontAtlas(const std::wstring fontFilePath, FontAtlas& atlasOut,
    float fontPx, int atlasW, int atlasH, int paddingPx)
{
    atlasOut = builder.BuildASCII(device.Get(), fontFilePath, fontPx, atlasW, atlasH, paddingPx);
}