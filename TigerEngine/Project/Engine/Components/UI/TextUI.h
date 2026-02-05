#pragma once
#include "../../Components/RenderComponent.h"
#include "../../Manager/UIData/TextResource.h"

struct GlyphDraw
{
    float x, y;   // 글리프 top-left (캔버스 좌표, y-down)
    float w, h;   // 글리프 bitmap size
    float u0, v0, u1, v1;
    int advance;
};

// 정렬 타입
enum class HAlign
{
    Left, Center, Right
};

class TextUI : public RenderComponent
{
public:
    void OnRender(RenderQueue& queue) override;

    /// <summary>
    /// 경로에 있는 폰트 가져오기
    /// </summary>
    void LoadFontAltas(const std::wstring fontFilePath, float fontPx, int atlasW, int altasH, int paddingPx);

    /// <summary>
    /// Builder로 재빌드가 필요한지, 추가해야하는지 확인해주고 설정
    /// </summary>
    void EnsureAltasForText(const std::vector<uint32_t>& cps);

    std::wstring GetText() const;
    void SetText(std::wstring wstr);

    Color GetColor() const;
    void SetColor(Color color);

    bool IsDirty() const;

private:
    std::shared_ptr<TextResource> resource{}; // atlas

    Color color{};          // 글자 색깔
    std::wstring text{};    // 출력할 텍스트
    
    HAlign alignType = HAlign::Left; // 폰트 정렬 타입
    std::wstring fontPath;			// 폰트 위치
    float fontPx = 0;				// 폰트 크기
    int atlasW = 0, atlasH = 0, paddingPx = 1; // 아틀라스 크기, 패딩 크기

    bool geometryDirty = true;
    uint32_t maxGlyphs = 256; // 초기값
};