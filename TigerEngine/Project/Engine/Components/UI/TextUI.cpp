#include "TextUI.h"
#include "../../Manager/UIManager.h"

void TextUI::OnRender(RenderQueue& queue)
{
}

void TextUI::LoadFontAltas(const std::wstring fontFilePath, float fontPx, int atlasW, int altasH, int paddingPx)
{
    // 혼트 정보 저장
    this->fontPath = fontFilePath;
    this->fontPx = fontPx;
    this->atlasW = atlasW;
    this->atlasH = atlasH;
    this->paddingPx = paddingPx;

    
    UIManager::Instance().LoadFontAtlas(fontFilePath, resource->atlas, fontPx, atlasW, atlasH, paddingPx);
}

void TextUI::EnsureAltasForText(const std::vector<uint32_t>& cps)
{
}

std::wstring TextUI::GetText() const
{
    return std::wstring();
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
