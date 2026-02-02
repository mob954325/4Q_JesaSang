#include "Image.h"
#include "../../Manager/TextureResourceManager.h"
#include "../../Object/GameObject.h"

void Image::GetTextureByPath(std::string path)
{
    resource = TextureResourceMaager::Instance().LoadTextureResourceByPath(path);
}

void Image::OnRender(RenderQueue& queue)
{
    if (!resource) return;

    // ItemData 채우기
    UIRenderItem data;
    data.color = color;

    Vector2 rectSize = rect->GetSize();
    data.imageSize = Vector4(rectSize.x, rectSize.y, resource->texSizePx.x, resource->texSizePx.y);

    data.params = Vector4((float)type, fillAmount, 0.0f, 0.0f);

    data.uvRect = sliceBorderPx;

    queue.AddUIRenderQueue(data); // NOTE : 이러면 sort를 renderpass에서 처리하면되니까 canvas는 왜 필요한거지
}

void Image::SetMouseCheck(bool value)
{
    isMouseCheck = value;
}

bool Image::GetMouseCheck() const
{
    return isMouseCheck;
}

void Image::Init()
{
    rect = GetOwner()->AddComponent<RectTransform>();
}
