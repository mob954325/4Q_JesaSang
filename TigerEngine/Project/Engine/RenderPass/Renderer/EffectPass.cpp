#include "EffectPass.h"
#include "../Renderable/DecalVolumeMesh.h"
#include "../../Manager/ShaderManager.h"
#include "../../EngineSystem/CameraSystem.h"
// TODO :: Effect 컴포넌트를 업데이트 하는 Effect System 필요
#include "../../Object/GameObject.h"

EffectPass::~EffectPass()
{
}

void EffectPass::Execute(ComPtr<ID3D11DeviceContext>& context,
    RenderQueue& queue, Camera* cam)
{

}

void EffectPass::Execute(ComPtr<ID3D11DeviceContext>& context, RenderQueue& queue, Camera* cam)
{
}
