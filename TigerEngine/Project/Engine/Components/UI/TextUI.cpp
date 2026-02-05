#include "TextUI.h"
#include "../../Manager/UIManager.h"
#include "../../Object/GameObject.h"
#include "../../../Base/Datas/ReflectionMedtaDatas.hpp"

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
            (metadata(META_INPUT, ""));
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

void TextUI::LoadFontAltas(const std::wstring fontFilePath, float fontPx, int atlasW, int atlasH, int paddingPx)
{
    // 혼트 정보 저장
    resource.reset();
    resource = std::make_shared<TextResource>();

    this->fontPath = fontFilePath;
    resource->fontPx = fontPx;
    resource->atlasW = atlasW;
    resource->atlasH = atlasH;
    resource->paddingPx = paddingPx;
    
    UIManager::Instance().LoadFontAtlas(fontFilePath, resource.get());

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