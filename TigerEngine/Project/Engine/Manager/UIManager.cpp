#include "UIManager.h"

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
