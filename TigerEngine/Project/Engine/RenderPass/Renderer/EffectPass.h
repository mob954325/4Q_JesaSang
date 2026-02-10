#pragma once
#include "../IRenderPass.h"
#include "../Renderable/ParticleQuadMesh.h"
#include "pch.h"

/*
    [ Effect Renderer ]

    Particle Renderer 코드를 참고하여 작성하세요
*/


class EffectPass : public IRenderPass
{
private:
    ParticleQuadMesh quad;
    UINT maxParticleInstances = 1024;
    ComPtr<ID3D11Buffer> instanceBuffer = nullptr;

public:
    ~EffectPass() override;

    void Init(const ComPtr<ID3D11Device>& device);
    void Execute(ComPtr<ID3D11DeviceContext>& context,
        RenderQueue& queue, Camera* cam) override;
    void End(ComPtr<ID3D11DeviceContext>& context) override {}

private:
    void EnsureInstanceCapacity(UINT required);
};
