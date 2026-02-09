#pragma once
#include "../IRenderPass.h"
#include "pch.h"

/*
    [ Effect Renderer ]

    Particle Renderer 코드를 참고하여 작성하세요
*/


class EffectPass : public IRenderPass
{
private:

public:
    ~EffectPass() override;

    void Init(const ComPtr<ID3D11Device>& device);
    void Execute(ComPtr<ID3D11DeviceContext>& context,
        RenderQueue& queue, Camera* cam) override;
    void End(ComPtr<ID3D11DeviceContext>& context) override {}
};

