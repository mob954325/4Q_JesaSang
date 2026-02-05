#include "TextUI.h"
#include "../../Manager/UIManager.h"
#include "../../Object/GameObject.h"
#include "../../../Base/Datas/ReflectionMedtaDatas.hpp"
#include "../../Util/JsonHelper.h"
#include "../Engine/EngineSystem/CameraSystem.h"

RTTR_REGISTRATION
{
    rttr::registration::enumeration<HAlign>("Align")
    (
        rttr::value("Center", HAlign::Center),
        rttr::value("Left", HAlign::Left),
        rttr::value("Right", HAlign::Right)
    );

rttr::registration::class_<TextUI>("TextUI")
        .constructor<>()
            (rttr::policy::ctor::as_std_shared_ptr)
                .property("alignType", &TextUI::alignType)
                .property("color", &TextUI::GetColor, &TextUI::SetColor)
                .property("fontPath", &TextUI::fontPath)
            (metadata(META_BROWSE, ""))
                .property("text", &TextUI::GetText, &TextUI::SetText)
            (metadata(META_INPUT, ""))
                .property("fontSize", &TextUI::GetFontSize, &TextUI::SetFontSize)
                .property("atlasW", &TextUI::atlasW)
                .property("atlasH", &TextUI::atlasH)
                .property("paddingPx", &TextUI::paddingPx)
                .property("drawSpacetype", &TextUI::GetDrawSpace, &TextUI::SetDrawSpace)
                .property("zOrder", &TextUI::GetZOrder, &TextUI::SetZOrder)
                .property("isBillboard", &TextUI::isBillboard);
}

void TextUI::OnRender(RenderQueue& queue)
{
    if (!resource) return;
    if (!resource->atlas.srv) return;

    auto rect = GetOwner()->GetComponent<RectTransform>();
    if (!rect) return;

    ImageUIRenderItem item;
    if (isBillboard)
    {
        auto tr = GetOwner()->GetTransform();
        Matrix R = GetScreenAlignedBillboardRotation();
        Vector3 pivot = { rect->GetPivot().x,rect->GetPivot().y, 0.0f };
        
        item.worldMat = 
            Matrix::CreateScale(tr->GetScale()) *
            R *
            Matrix::CreateTranslation(tr->GetWorldPosition());
    }
    else
    {
        item.worldMat = GetOwner()->GetTransform()->GetWorldMatrix();
    }

    item.isText = true;
    item.textComp = this;
    item.geometryDirty = geometryDirty; // Note : rebuild geometry를 위한 플래그 
    item.color = color;
    item.screenMat = rect->GetWorldMatrix();
    item.imageSize = Vector4(rect->GetSize().x, rect->GetSize().y, atlasW, atlasH); // width, height?
    item.params = { 0,0, rect->GetPivot().x, rect->GetPivot().y };
    if (drawSpacetype == DrawSpaceType::World)
        item.isWorldSpace = true;
    else
        item.isWorldSpace = false;

    item.zOrder = zOrder;

    queue.AddUIRenderQueue(item);

    if (geometryDirty) geometryDirty = false;
}

void TextUI::LoadFontAtlas(const std::wstring fontFilePath, int atlasW, int atlasH, int paddingPx)
{
    // 혼트 정보 저장
    if (!resource) resource = std::make_shared<TextResource>();

    fontPath = fontFilePath;
    this->atlasW = atlasW;
    this->atlasH = atlasH;
    this->paddingPx = paddingPx;

    resource->atlasW = atlasW;
    resource->atlasH = atlasH;
    resource->paddingPx = paddingPx;
    resource->fontPx = (float)fontSize;

    UIManager::Instance().LoadFontAtlas(fontPath, resource.get());
    geometryDirty = true;
}

std::wstring TextUI::GetText() const
{
    return text;
}

void TextUI::SetText(std::wstring wstr)
{
    text = std::move(wstr);
    geometryDirty = true;
}

Color TextUI::GetColor() const
{
    return color;
}

void TextUI::SetColor(Color color)
{
    this->color = color;
    geometryDirty = true;
}

bool TextUI::IsDirty() const
{
    return geometryDirty;
}

const std::shared_ptr<TextResource>& TextUI::GetResoucre()
{
    return resource;
}

int TextUI::GetFontSize() const
{
    return fontSize;
}

void TextUI::SetFontSize(int px)
{
    if (px < 1)
    {
        fontSize = 1;
    }

    fontSize = px;
    if (!resource) return;

    const int prevPx = fontSize;
    resource->fontPx = (float)px;

    // 기존 설정 유지
    resource->atlasW = atlasW;
    resource->atlasH = atlasH;
    resource->paddingPx = paddingPx;

    // 텍스트 내용에 따라 최대 px는 런타임에서 달라지기 때문에 정확한 수식 계산하기가 힘들다.
    // 따라서 시도 해보고 실패하면 롤백 그 외는 runtimeError 호출
    try
    {
        UIManager::Instance().LoadFontAtlas(fontPath, resource.get());

        // 성공했으면 geometry만 갱신
        geometryDirty = true;
    }
    catch (const std::runtime_error& e)
    {
        // UIManager가 2048까지 키워도 "atlas full"이면 더 못 키우는 상황
        if (std::string(e.what()) == "atlas full")
        {
            // 폰트 크기 증가를 무효화 (최대치로 고정 효과)
            fontSize = prevPx;
            resource->fontPx = (float)prevPx;

            // 이전 크기로 다시 빌드해서 상태 복구
            UIManager::Instance().LoadFontAtlas(fontPath, resource.get());
            geometryDirty = true;
            return;
        }

        throw; // 다른 에러는 그대로
    }

    geometryDirty = true;
}

nlohmann::json TextUI::Serialize()
{
    return JsonHelper::MakeSaveData(this);
}

void TextUI::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);

    LoadFontAtlas(fontPath, atlasW, atlasH, paddingPx);
    geometryDirty = true;
}

int TextUI::GetZOrder() const
{
    return zOrder;
}

void TextUI::SetZOrder(int v)
{
    zOrder = v;
}

Matrix TextUI::GetScreenAlignedBillboardRotation()
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

Matrix TextUI::BuildBillboardWorldMatrix()
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
