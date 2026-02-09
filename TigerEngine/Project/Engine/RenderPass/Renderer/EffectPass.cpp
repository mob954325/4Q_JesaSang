#include "EffectPass.h"
#include "../../Manager/ShaderManager.h"
#include "../../EngineSystem/EffectSystem.h"
#include "../ParticleSource/Effect.h"
#include "../ParticleSource/Emitter.h"
#include "../ParticleSource/Particle.h"
#include "../../Object/GameObject.h"

#include "../../../Base/Datas/Vertex.h"

#include <algorithm>
#include <climits>
#include <cstring>
#include <vector>

using std::vector;

EffectPass::~EffectPass() = default;

void EffectPass::Init(const ComPtr<ID3D11Device>& device)
{
    quad.Create(device);

    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = sizeof(ParticleInstance) * maxParticleInstances;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    device->CreateBuffer(&desc, nullptr, instanceBuffer.ReleaseAndGetAddressOf());
}

void EffectPass::Execute(ComPtr<ID3D11DeviceContext>& context, RenderQueue& queue, Camera* cam)
{
    (void)queue;
    if (!cam) return;

    auto& sm = ShaderManager::Instance();

    // Update (Effect component doesn't receive OnUpdate)
    auto effects = EffectSystem::Instance().GetComponents();
    for (Effect* fx : effects)
    {
        if (!fx) continue;
        fx->Update();
    }

    // Sort (Back-to-Front)
    vector<FxSortItem> sortedFx;
    sortedFx.reserve(effects.size());

    auto* camOwner = cam->GetOwner();
    if (!camOwner || !camOwner->GetTransform()) return;

    const Vector3 camPos = camOwner->GetTransform()->GetLocalPosition();
    const Vector3 camForward = cam->GetForward();

    for (Effect* fx : effects)
    {
        if (!fx) continue;
        if (!fx->enabled) continue;

        bool hasAnyParticles = false;
        for (const auto& em : fx->emitters)
        {
            if (!em.enabled) continue;
            if (!em.particles.empty())
            {
                hasAnyParticles = true;
                break;
            }
        }
        if (!hasAnyParticles) continue;

        Vector3 dist = fx->position - camPos;
        float key = dist.Dot(camForward);
        sortedFx.push_back({ fx, key });
    }

    std::sort(sortedFx.begin(), sortedFx.end(),
        [](const FxSortItem& a, const FxSortItem& b) { return a.key > b.key; });

    if (sortedFx.empty()) return;

    // RTV, DSV
    context->RSSetViewports(1, &sm.viewport_screen);
    context->OMSetRenderTargets(1, sm.sceneHDRRTV.GetAddressOf(), sm.depthStencilReadOnlyView.Get());

    // IA
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(sm.inputLayout_Particle.Get());

    // DSS (depth test only)
    context->OMSetDepthStencilState(sm.depthTestOnlyDSS.Get(), 0);

    // RS
    context->RSSetState(sm.cullNoneRS.Get());

    // Shader
    context->VSSetShader(sm.VS_Effect.Get(), nullptr, 0);
    context->PSSetShader(sm.PS_Effect.Get(), nullptr, 0);

    // Sampler
    context->PSSetSamplers(0, 1, sm.linearSamplerState.GetAddressOf());

    // Blend State (alpha)
    float blendFactor[4] = { 0,0,0,0 };
    context->OMSetBlendState(sm.alphaBlendState.Get(), blendFactor, 0xffffffff);

    // CB - Transform (view/projection only)
    sm.transformCBData.view = XMMatrixTranspose(cam->GetView());
    sm.transformCBData.projection = XMMatrixTranspose(cam->GetProjection());
    context->UpdateSubresource(sm.transformCB.Get(), 0, nullptr, &sm.transformCBData, 0, 0);

    vector<ParticleInstance> instances;
    instances.reserve(256);

    // Render
    for (const auto& item : sortedFx)
    {
        const Effect* fx = item.fx;
        if (!fx) continue;

        for (const auto& em : fx->emitters)
        {
            if (!em.enabled) continue;
            if (em.particles.empty()) continue;
            if (!em.sheet.resource || !em.sheet.resource->srv) continue;

            // CB - Effect
            const float cols = (float)std::max(1, em.sheet.cols);
            const float rows = (float)std::max(1, em.sheet.rows);
            sm.effectCBData.atlasGrid = { cols , rows };
            sm.effectCBData.invAtlasGrid = { 1.0f / cols, 1.0f / rows };
            sm.effectCBData.baseSizeScale = em.sheet.baseSizeScale;
            sm.effectCBData.billboardType = (int)em.billboard;
            context->UpdateSubresource(sm.effectCB.Get(), 0, nullptr, &sm.effectCBData, 0, 0);

            // SRV (t18)
            ID3D11ShaderResourceView* effectSrv = em.sheet.resource->srv.Get();
            context->PSSetShaderResources(18, 1, &effectSrv);

            // Instance Buffer
            instances.clear();
            instances.reserve(em.particles.size());

            for (const auto& p : em.particles)
            {
                ParticleInstance i{};
                i.pos = p.pos;
                i.rotation = p.rotation;
                i.size = p.size;
                i.frame = (float)p.frame;
                i.color = p.color;
                instances.push_back(i);
            }

            if (instances.empty()) continue;

            EnsureInstanceCapacity((UINT)instances.size());
            if ((UINT)instances.size() > maxParticleInstances) continue;

            D3D11_MAPPED_SUBRESOURCE mapped{};
            context->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
            memcpy(mapped.pData, instances.data(), instances.size() * sizeof(ParticleInstance));
            context->Unmap(instanceBuffer.Get(), 0);

            quad.DrawIndexedInstanced(context, (UINT)instances.size(),
                instanceBuffer.Get(), sizeof(ParticleInstance));
        }
    }

    // clean up
    context->RSSetState(nullptr);
    context->OMSetDepthStencilState(nullptr, 0);
    context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    context->PSSetShaderResources(18, 1, nullSRV); // effectTex
}

void EffectPass::EnsureInstanceCapacity(UINT required)
{
    if (required <= maxParticleInstances) return;

    UINT newCap = maxParticleInstances;
    while (newCap < required)
    {
        if (newCap > (UINT_MAX / 2))
        {
            newCap = required;
            break;
        }
        newCap *= 2;
    }

    auto& sm = ShaderManager::Instance();
    if (!sm.device) return;

    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = sizeof(ParticleInstance) * newCap;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ComPtr<ID3D11Buffer> newBuffer;
    HRESULT hr = sm.device->CreateBuffer(&desc, nullptr, newBuffer.GetAddressOf());
    if (FAILED(hr)) return;

    instanceBuffer = newBuffer;
    maxParticleInstances = newCap;
}
