#pragma once
#include "../../Components/RenderComponent.h"
#include "../../Manager/UIData/TextResource.h"
#include "UITextDatas.h"
#include "../../RenderPass/Renderable/UIQuadVertex.h"

class TextUI : public RenderComponent
{
    RTTR_ENABLE(RenderComponent)
public:
    void OnRender(RenderQueue& queue) override;

    /// <summary>
    /// 경로에 있는 폰트 가져오기
    /// </summary>
    void LoadFontAltas(const std::wstring fontFilePath, float fontPx = 32, int atlasW = 2048, int atlasH = 2048, int paddingPx = 1);

    std::wstring GetText() const;
    void SetText(std::wstring wstr);

    Color GetColor() const;
    void SetColor(Color color);

    bool IsDirty() const;

    const std::shared_ptr<TextResource>& GetResoucre();

    // 자원
    std::shared_ptr<TextResource> resource{}; // atlas

    Color color{};          // 글자 색깔
    std::wstring text{};    // 출력할 텍스트
    
    HAlign alignType = HAlign::Left; // 폰트 정렬 타입
    std::wstring fontPath;			// 폰트 위치

    bool geometryDirty = true;
    uint32_t maxGlyphs = 256; // 초기값

    std::vector<UIQuadVertex> cpuVerts{};
    int indexCount = 0;
};