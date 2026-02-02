#pragma once
#include "../IRenderPass.h"

class UIRenderPass : public IRenderPass
{
public:
    void Execute(
        ComPtr<ID3D11DeviceContext>& context,
        RenderQueue& queue,
        Camera* cam
    ) override;

    void End(ComPtr<ID3D11DeviceContext>& context);
};
