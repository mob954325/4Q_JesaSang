#include "TextUI.h"
#include "../../Manager/UIManager.h"
#include "../../Object/GameObject.h"

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

    queue.AddUIRenderQueue(item);
}

void TextUI::LoadFontAltas(const std::wstring fontFilePath, float fontPx, int atlasW, int atlasH, int paddingPx)
{
    // 혼트 정보 저장
    this->fontPath = fontFilePath;
    resource->fontPx = fontPx;
    resource->atlasW = atlasW;
    resource->atlasH = atlasH;
    resource->paddingPx = paddingPx;

    
    UIManager::Instance().LoadFontAtlas(fontFilePath, resource.get());
}

std::wstring TextUI::GetText() const
{
    return text;
}

void TextUI::SetText(std::wstring wstr)
{
    text = wstr;
}

Color TextUI::GetColor() const
{
    return color;
}

void TextUI::SetColor(Color color)
{
    this->color = color;
}

bool TextUI::IsDirty() const
{
    return geometryDirty;
}
