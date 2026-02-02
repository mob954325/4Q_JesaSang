#pragma once
#include "../RenderComponent.h"

class Image : public RenderComponent
{
    RTTR_ENABLE(RenderComponent)
public:
    void GetTextureByPath(std::string path);

    void OnRender(RenderQueue& queue) override;
};
