#include "UIRenderPass.h"
#include "../../Manager/ShaderManager.h"
#include "../../Manager/UIManager.h"

void UIRenderPass::Init(const ComPtr<ID3D11Device>& device)
{
    mesh.Create(device);
}

void UIRenderPass::Execute(ComPtr<ID3D11DeviceContext>& context, RenderQueue& queue, Camera* cam)
{
    auto& sm = ShaderManager::Instance();
    auto& um = UIManager::Instance();

    context->OMSetBlendState(sm.alphaBlendState.Get(), nullptr, 0xffffffff);	//

    // ia
    auto& indexBuffer = mesh.GetIndexBuffer();
    auto& vertexBuffer = mesh.GetVertexBuffer();

    context->IASetInputLayout(sm.inputLayout_ui.Get());
    context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    UINT stride = mesh.GetStride();
    UINT offset = mesh.GetOffset();
    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

    // vs rs
    context->PSSetSamplers(0, 1, sm.linearSamplerState.GetAddressOf());
    context->RSSetState(sm.cullNoneRS.Get());
    context->VSSetShader(sm.VS_UIImage.Get(), nullptr, 0);

    context->OMSetDepthStencilState(sm.disableDSS.Get(), 1);

    auto& renderQueue = queue.GetUIRenderQueue();
    for (auto& item : renderQueue)
    {
        Matrix mvp = item.worldMat * um.GetProjection();	// UI에서  view는 보통 identity
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
        context->PSSetShader(sm.PS_UIImage.Get(), nullptr, 0);					// ps 바인딩

        context->PSSetShaderResources(20, 1, item.resource->srv.GetAddressOf());			// 텍스처 리소스 바인딩

        context->DrawIndexed(6, 0, 0);	// 쿼드 그리기
    }   
}

void UIRenderPass::End(ComPtr<ID3D11DeviceContext>& context)
{
    context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}
