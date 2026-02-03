#include "Image.h"
#include "../../Manager/TextureResourceManager.h"
#include "../../Object/GameObject.h"
#include "../../Util/JsonHelper.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"

RTTR_REGISTRATION
{
    rttr::registration::enumeration<ImageType>("ImageType")
    (
        rttr::value("Simple", ImageType::Simple),
        rttr::value("Fill", ImageType::Fill),
        rttr::value("Sliced", ImageType::Sliced)
    );

    rttr::registration::class_<Image>("Image")
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr)
        .property("type",           &Image::GetType,        &Image::SetType)
        .property("color",          &Image::GetColor,       &Image::SetColor)
        .property("fillAmount",     &Image::GetFillAmount,  &Image::SetFillAmount)
        .property("sliceBorderPx",  &Image::GetBorderPx,    &Image::SetBorderPx)
        .property("path",           &Image::GetPath,        &Image::SetPath)
            (metadata(META_BROWSE, ""))
        .property("isMouseCheck",   &Image::GetMouseCheck,  &Image::SetMouseCheck);
}

void Image::OnInitialize()
{
    Init();
}

nlohmann::json Image::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void Image::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);

    resource.reset();
    GetTextureByPath(path);
}

void Image::GetTextureByPath(std::string path)
{
    resource = TextureResourceManager::Instance().LoadTextureResourceByPath(path);
}

void Image::OnRender(RenderQueue& queue)
{
    if (!resource) return;

    // ItemData 채우기
    UIRenderItem data;
    auto rect = GetOwner()->GetComponent<RectTransform>();
    if (!rect) return;

    data.worldMat = rect->GetWorldMatrix();
    data.color = color;

    Vector2 rectSize = rect->GetSize();
    data.imageSize = Vector4(rectSize.x, rectSize.y, resource->texSizePx.x, resource->texSizePx.y);
    data.params = Vector4((float)type, fillAmount, 0.0f, 0.0f);
    data.uvRect = sliceBorderPx;
    data.resource = resource.get();
    data.isText = false;

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

void Image::ChangeData(std::string path)
{
    resource.reset();
    resource = TextureResourceManager::Instance().LoadTextureResourceByPath(path);
    this->path = path;
}

void Image::Init()
{
    // if(!GetOwner()->GetComponent<RectTransform>())
    //     rect = GetOwner()->AddComponent<RectTransform>();
}