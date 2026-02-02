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

void UIManager::Register(UIBase* ui)
{
    uiComps.push_back(ui);
}

void UIManager::UnRegister(UIBase* ui)
{
    for (auto it = uiComps.begin(); it != uiComps.end();)
    {
        if (*it == ui)
        {
            uiComps.erase(it);
            break;
        }
        else
        {
            it++;
        }
    }
}
