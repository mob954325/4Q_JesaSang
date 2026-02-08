#pragma once
#include "../../Components/RenderComponent.h"
#include "../../Manager/UIData/TextResource.h"
#include "UITextDatas.h"
#include "../../RenderPass/Renderable/UIQuadVertex.h"
#include "UITypeDatas.h"

/// <summary>
/// text 그리는 컴포넌트
/// 최초 폰트 경로를 얻을 때 리소스가 생성되며 이후 .reset을 사용해 초기화 후 갱신
/// </summary>
class TextUI : public RenderComponent
{
    RTTR_ENABLE(RenderComponent)
public:
    void OnRender(RenderQueue& queue) override;

    /// <summary>
    /// 경로에 있는 폰트 가져오기
    /// </summary>
    void LoadFontAtlas(const std::wstring fontFilePath, int atlasW = 2048, int atlasH = 2048, int paddingPx = 1);

    std::wstring GetText() const;
    void SetText(std::wstring wstr);

    Color GetColor() const;
    void SetColor(Color color);

    bool IsDirty() const;

    const std::shared_ptr<TextResource>& GetResoucre();

    int GetFontSize() const;
    void SetFontSize(int px); // NOTE 아틀라스 크기보다 폰트양이 많으면 터지기 때문에 적당히 키운다. 

    DrawSpaceType GetDrawSpace() const { return drawSpacetype; }
    void SetDrawSpace(DrawSpaceType value) { drawSpacetype = value; }

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    int GetZOrder() const;
    void SetZOrder(int v);

// Private:
    // 자원
    std::shared_ptr<TextResource> resource{}; // atlas

    Color color{};          // 글자 색깔
    std::wstring text{};    // 출력할 텍스트

    DrawSpaceType drawSpacetype = DrawSpaceType::Screen;
    HAlign alignType = HAlign::Left; // 폰트 정렬 타입
    std::wstring fontPath;			// 폰트 위치

    int fontSize = 1; // px
    int atlasW = 2048;
    int atlasH = 2048;
    int paddingPx = 1;

    bool geometryDirty = true;
    uint32_t maxGlyphs = 256; // 초기값

    std::vector<UIQuadVertex> cpuVerts{};
    int indexCount = 0;

    int zOrder = 0;

    bool isBillboard = false;

    Matrix GetScreenAlignedBillboardRotation();
    Matrix BuildBillboardWorldMatrix();
};