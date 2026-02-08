#include "Image.h"
#include "../../Manager/TextureResourceManager.h"
#include "../../Object/GameObject.h"
#include "../../Util/JsonHelper.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"
#include "../Engine/EngineSystem/CameraSystem.h"

#include <directXTK/Mouse.h>
#include <directXTK/Keyboard.h>

RTTR_REGISTRATION
{
    rttr::registration::enumeration<ImageType>("ImageType")
    (
        rttr::value("Simple", ImageType::Simple),
        rttr::value("Fill", ImageType::Fill),
        rttr::value("Sliced", ImageType::Sliced),
        rttr::value("FillHorizontal", ImageType::FillHorizontal)
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
        .property("isMouseCheck",   &Image::GetMouseEvnetActive,  &Image::SetMouseEventActive)
        .property("imageType",      &Image::GetType,        &Image::SetType)
        .property("drawSpacetype",  &Image::GetDrawSpace,  &Image::SetDrawSpace)
        .property("isMouseCheck",   &Image::GetMouseEvnetActive,  &Image::SetMouseEventActive)
        .property("zOrder",         &Image::GetZOrder, &Image::SetZOrder)
        .property("isBillboard",    &Image::isBillboard);
}

void Image::OnInitialize()
{
    Init();
}

void Image::OnUpdate(float delta)
{
    if (useMouseEvnet)
    {
        CheckMouseHover();

        auto ms = DirectX::Mouse::Get().GetState();
        bool curLeft = (ms.leftButton != 0);
        if (!hoverd && isMouseHover)
        {
            OnEnter.Invoke();
            hoverd = true;
        }

        if (curLeft && !prevLeft && isMouseHover)
        {
            // 여기서 "좌클릭 1회" 처리 (Pressed 순간)
            std::cout << "image Clicked!!!\n";
            OnClick.Invoke();
            isClick = true;
        }

        if (curLeft && isMouseHover)
        {
            std::cout << "image pressed!!!\n";
            OnPressed.Invoke();
        }

        if (!curLeft && isClick && prevLeft && isMouseHover)
        {
            std::cout << "image pressedout aaa\n";
            OnPressOut.Invoke();
            isClick = false;
        }

        if (hoverd && !isMouseHover)
        {
            if (isClick) // 누르면서 버튼 나가면 out 처리
            {
                std::cout << "image click hover out aaa\n";
                OnPressOut.Invoke();
                isClick = false;
            }

            std::cout << "image hover out aaa\n";
            OnExit.Invoke();
            hoverd = false;
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

    if (isBillboard)
    {
        data.worldMat = BuildBillboardWorldMatrix();
    }
    else
    {
        data.worldMat = GetOwner()->GetTransform()->GetWorldMatrix();
    }

    data.screenMat = rect->GetWorldMatrix();
    data.color = color;

    Vector2 rectSize = rect->GetSize();
    data.imageSize = Vector4(rectSize.x, rectSize.y, resource->texSizePx.x, resource->texSizePx.y);
    data.params = Vector4((float)type, fillAmount, rect->GetPivot().x, rect->GetPivot().y);
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

void Image::SetMouseEventActive(bool value)
{
    useMouseEvnet = value;
}

bool Image::GetMouseEvnetActive() const
{
    return useMouseEvnet;
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

Matrix Image::GetScreenAlignedBillboardRotation()
{
    auto cam = CameraSystem::Instance().GetCurrCamera();

    // 카메라의 월드 회전 = view^-1
    Matrix invView = cam->GetView().Invert();
    invView.Translation(Vector3::Zero); // 회전만 사용

    //invView = Matrix::CreateRotationY(DirectX::XM_PI) * invView;
    invView = Matrix::CreateRotationX(DirectX::XM_PI) * invView;

    return invView;
}

static Vector3 ExtractWorldScale(const Matrix& W)
{
    // W의 basis 벡터(축) 길이 = 스케일
    // SimpleMath는 Right/Up/Forward 접근자가 있는 경우가 많음.
    Vector3 right = W.Right();
    Vector3 up = W.Up();
    Vector3 forward = W.Forward();

    float sx = right.Length();
    float sy = up.Length();
    float sz = forward.Length();

    // 스케일 0 방지
    if (sx < 1e-8f) sx = 1.0f;
    if (sy < 1e-8f) sy = 1.0f;
    if (sz < 1e-8f) sz = 1.0f;

    return Vector3(sx, sy, sz);
}

Matrix Image::BuildBillboardWorldMatrix()
{
    auto tr = GetOwner()->GetTransform();

    Matrix W0 = tr->GetWorldMatrix();               // 기존 월드(부모 포함)
    Vector3 ws = ExtractWorldScale(W0);             // 월드 스케일

    Matrix R = GetScreenAlignedBillboardRotation(); // 회전만(translation=0)

    Matrix W = Matrix::CreateScale(ws) * R;         // 스케일+빌보드 회전

    // 위치는 기존 월드 위치로 강제
    W.Translation(tr->GetWorldPosition());
    return W;
}
