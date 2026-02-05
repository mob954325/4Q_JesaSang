#include "UIRenderPass.h"
#include "../../Manager/ShaderManager.h"
#include "../../Manager/UIManager.h"
#include "../../Object/GameObject.h"
#include "../../Components/UI/TextUI.h"
#include "../../EngineSystem/CameraSystem.h"

void UIRenderPass::Init(const ComPtr<ID3D11Device>& device)
{
    mesh.Create(device);
    mesh.CreateTextBuffers(512);
}

void UIRenderPass::Execute(ComPtr<ID3D11DeviceContext>& context, RenderQueue& queue, Camera* cam)
{
    auto& sm = ShaderManager::Instance();
    auto& um = UIManager::Instance();

    context->OMSetBlendState(sm.alphaBlendState.Get(), nullptr, 0xffffffff);	//

    // ia
    context->IASetInputLayout(sm.inputLayout_ui.Get());

    // vs rs
    context->PSSetSamplers(0, 1, sm.linearSamplerState.GetAddressOf());
    context->RSSetState(sm.cullNoneRS.Get());
    context->VSSetShader(sm.VS_UIImage.Get(), nullptr, 0);

    context->OMSetDepthStencilState(sm.disableDSS.Get(), 1);

    auto& renderQueue = queue.GetUIRenderQueue();
    for (auto& item : renderQueue)
    {
        Matrix mvp;
        if (!item.isWorldSpace)
            mvp = item.worldMat * um.GetProjection();	// UI에서  view는 보통 identity
        else
        {
            Camera* cam = CameraSystem::Instance().GetCurrCamera();
            mvp = item.worldMat * cam->GetView() * cam->GetProjection();
        }

        sm.uiCBData.WVP = mvp.Transpose();
        sm.uiCBData.color = item.color;

        // type / fillAmount 전달하기        
        sm.uiCBData.params = Vector4(item.params.x, item.params.y, 0.0f, 0.0f);

        // uvRect : 9-sliced 보더(px)
        sm.uiCBData.uvRect = item.uvRect;

        // imageSize : rectW/ rectH/ texW/ texH
        sm.uiCBData.imageSize = item.imageSize;

        context->UpdateSubresource(sm.uiCB.Get(), 0, nullptr, &sm.uiCBData, 0, 0); // 상수 버퍼 업데이트

        context->VSSetConstantBuffers(11, 1, sm.uiCB.GetAddressOf());	// vs 상수 버퍼 설정
        context->PSSetConstantBuffers(11, 1, sm.uiCB.GetAddressOf());	// ps 상수 버퍼 설정

        if (!item.isText)
        {
            auto& indexBuffer = mesh.GetIndexBuffer();
            auto& vertexBuffer = mesh.GetVertexBuffer();

            UINT stride = mesh.GetStride();
            UINT offset = mesh.GetOffset();

            context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
            context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

            context->PSSetShader(sm.PS_UIImage.Get(), nullptr, 0);					// ps 바인딩
            context->PSSetShaderResources(20, 1, item.resource->srv.GetAddressOf());			// 텍스처 리소스 바인딩
            context->DrawIndexed(6, 0, 0);	// 쿼드 그리기
        }
        else
        {
            TextUI* t = item.textComp;
            auto resource = t->GetResoucre();
            if (!t || !resource || !resource->atlas.srv) continue; // 자원이 존재하지 않음

            int fontPx = t->fontSize;
            Vector2 rectSize = Vector2(item.imageSize.x, item.imageSize.y);

            if (item.geometryDirty)
            {
                UIManager::Instance().RebuildGeometry(
                    t->fontPath,             // path
                    t->GetText(),            // text
                    rectSize,                // size
                    t->alignType,            // align
                    t->resource.get(),
                    t->cpuVerts,
                    t->indexCount
                );
            }

            if (t->indexCount == 0 || t->cpuVerts.empty()) continue;

            uint32_t glyphCount = (uint32_t)(t->cpuVerts.size() / 4);

            // 텍스트 파이프라인 바인딩 (PS만 다르게)
            UINT stride = sizeof(UIQuadVertex);
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, mesh.textVB.GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(mesh.textIB.Get(), DXGI_FORMAT_R16_UINT, 0);

            mesh.EnsureTextCapacity(glyphCount);
            mesh.UploadTextVB(context, t->cpuVerts);

            context->PSSetShader(sm.PS_UIText.Get(), nullptr, 0); // 알파 텍스트용 PS
            context->PSSetShaderResources(21, 1, t->resource->atlas.srv.GetAddressOf());

            context->DrawIndexed((UINT)t->indexCount, 0, 0);
        }
    }   
}

void UIRenderPass::End(ComPtr<ID3D11DeviceContext>& context)
{
    context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}
