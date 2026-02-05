#pragma once
#include "../IRenderPass.h"
#include "../Renderable/UIQuadMesh.h"

class UIRenderPass : public IRenderPass
{
public:
    void Init(const ComPtr<ID3D11Device>& device);

    void Execute(ComPtr<ID3D11DeviceContext>& context,
                RenderQueue& queue,
                Camera* cam) override;

    void End(ComPtr<ID3D11DeviceContext>& context);

private:
    UIQuadMesh mesh;
};
