#pragma once
#include "../Base/pch.h"
#include "../Components/UI/UIBase.h"
#include "System/Singleton.h"
#include "UIData/TextResource.h"
#include "../RenderPass/Renderable/UIQuadMesh.h"
#include "../Components/UI/UITextDatas.h"

/// <summary>
/// 자식 계층중 UIBase를 가지고 있는 게임 오브젝트를 관리하는 매니저
/// 특수한 컴포넌트 클래스로 해상도, 스케일 정책을 공유하고, sorting 등을 수행합니다.
/// </summary>
class UIManager : public Singleton<UIManager>
{
public:
    UIManager(token) {};
    ~UIManager() = default;

    void Init(const ComPtr<ID3D11Device>& dev, const ComPtr<ID3D11DeviceContext>& ctx);

    /// <summary>
    /// client screen size 가져오기, 카메라 투영 계산도 포함됨.
    /// </summary>
    void SetSize(int w, int h);
    Vector2 GetSize();

    Matrix GetProjection() const;


    // Text Util
    void LoadFontAtlas( const std::wstring fontFilePath, TextResource* resource);
    void RebuildGeometry(std::wstring path,                         // 폰트 경로
                         std::wstring wstr,                         // textui 글자
                         Vector2& offset,                           // offset
                         HAlign alignType,                          // 정렬 타입
                         TextResource* resource,                    // textui 리소스
                         std::vector<UIQuadVertex>& cpuVerts,       // 정점 데이터
                         Vector2& pivot,                            // 피벗
                         int& outIndexCount);                       // [out] 인덱스 개수

protected:
    ComPtr<ID3D11Device> device{};
    ComPtr<ID3D11DeviceContext> context{};
    // std::vector<UIBase*> uiComps;    

    // UI ㅡMVP 계산 할 때 사용하는 투영 매트릭스 ( 직교 투영 )
    Matrix screenProj;  // orthographic

    int width{};
    int height{};

    FontAtlasBuilder builder{};

    // builder에 재 빌드가 필요한지, 추가를 해야하는지 확인하고 설정해주는 함수임
    void EnsureAtlasForText(ComPtr<ID3D11Device>& dev,
        TextResource* resource,
        std::wstring path,
        const std::vector<uint32_t>& cps);

    /// <summary>
    /// 출력할 width 계산 함수 ( RebuildGeometry 기능 분리 함수 )
    /// </summary>
    float MeasureWidthCP(TextResource* resource, const std::vector<uint32_t>& cps, size_t b, size_t e);

    /// <summary>
    /// 글자 쿼드 갱신 ( RebuildGeometry 기능 분리 함수 )
    /// </summary>
    void AppendGlyphQuad(float penX, float baselineY, std::vector<UIQuadVertex>& cpuVerts, const GlyphInfo& g); // third : glyphInfo
};