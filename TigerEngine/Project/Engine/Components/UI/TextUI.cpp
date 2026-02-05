#include "TextUI.h"
#include "../../Manager/UIManager.h"
#include "../../Object/GameObject.h"
#include "../../../Base/Datas/ReflectionMedtaDatas.hpp"
#include "../../Util/JsonHelper.h"

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
                .property("paddingPx", &TextUI::paddingPx);
}

void TextUI::OnRender(RenderQueue& queue)
{
    if (!resource) return;
    if (!resource->atlas.srv) return;

    auto rect = GetOwner()->GetComponent<RectTransform>();
    if (!rect) return;

    ImageUIRenderItem item;
    item.isText = true;
    item.textComp = this;
    item.geometryDirty = geometryDirty; // Note : rebuild geometry를 위한 플래그 
    item.color = color;
    item.worldMat = rect->GetWorldMatrix();
    item.imageSize = Vector4(rect->GetSize().x, rect->GetSize().y, 0, 0);

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