#include "Image.h"
#include "../../Manager/TextureResourceManager.h"
#include "../../Object/GameObject.h"
#include "../../Util/JsonHelper.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"

#include <directXTK/Mouse.h>
#include <directXTK/Keyboard.h>

RTTR_REGISTRATION
{
    rttr::registration::enumeration<ImageType>("ImageType")
    (
        rttr::value("Simple", ImageType::Simple),
        rttr::value("Fill", ImageType::Fill),
        rttr::value("Sliced", ImageType::Sliced)
    );

    rttr::registration::enumeration<DrawSpaceType>("DrawSpaceType")
    (
        rttr::value("Screen", DrawSpaceType::Screen),
        rttr::value("World", DrawSpaceType::World)
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
        .property("isMouseCheck",   &Image::GetMouseCheck,  &Image::SetMouseCheck)
        .property("imageType",      &Image::GetType,        &Image::SetType)
        .property("drawSpacetype",  &Image::GetDrawSpace,  &Image::SetDrawSpace)
        .property("isMouseCheck",   &Image::GetMouseCheck,  &Image::SetMouseCheck)
        .property("zOrder",         &Image::GetZOrder, &Image::SetZOrder);
}

void Image::OnInitialize()
{
    Init();
}

void Image::OnUpdate(float delta)
{
    if (isMouseCheck)
    {
        CheckMouseHover();

        auto ms = DirectX::Mouse::Get().GetState();
        bool curLeft = (ms.leftButton != 0);

        if (curLeft && !prevLeft && isMouseHover)
        {
            // 여기서 "좌클릭 1회" 처리 (Pressed 순간)
            std::cout << "image Clicked!!!\n";
        }

        prevLeft = curLeft;
    }
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
    if (!resource->srv) return;

    // ItemData 채우기
    ImageUIRenderItem data;
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

    if (drawSpacetype == DrawSpaceType::World)
        data.isWorldSpace = true;
    else
        data.isWorldSpace = false;

    data.zOrder = zOrder;

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

int Image::GetZOrder() const
{
    return zOrder;
}

void Image::SetZOrder(int v)
{
    zOrder = v;
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

void Image::CheckMouseHover()
{
    // Note : Mouse 충돌 테스트
    // 접근 : 마우스 위치를 로컬 쿼드 좌표로 옮겨서 내부 판정을 시작한다. ( world.invert )
    //		쿼드가 유닛값이므로 (0-1)로 내부판정을 한다.
    int mouseX = DirectX::Mouse::Get().GetState().x;
    int mouseY = DirectX::Mouse::Get().GetState().y;

    auto rect = GetOwner()->GetComponent<RectTransform>();

    if (!rect) return;

    Matrix invWorld = rect->GetWorldMatrix().Invert();
    Vector3 mouseWorld(mouseX, mouseY, 0.0f);
    Vector3 local = Vector3::Transform(mouseWorld, invWorld);

    // 유닛 쿼드 내부 판정 (0-1)
    isMouseHover = (local.x >= 0.0f && local.x <= 1.0f) &&
        (local.y >= 0.0f && local.y <= 1.0f);
}
