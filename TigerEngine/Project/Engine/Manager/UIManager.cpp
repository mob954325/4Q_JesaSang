#include "UIManager.h"
#include "../EngineSystem/CameraSystem.h"

// === Util ===
// UTF16 CodePoint 찾기
static void DecodeUTF16ToCodepoints(const std::wstring& s, std::vector<uint32_t>& out)
{
    out.clear();
    out.reserve(s.size());

    // Windows wchar_t는 UTF-16. 한글은 BMP라 대부분 1개 wchar_t지만
    // 이모지 등 surrogate pair도 안전하게 처리.
    for (size_t i = 0; i < s.size(); ++i)
    {
        uint32_t wc = (uint32_t)s[i]; // 한글자 값 코드

        if (wc >= 0xD800 && wc <= 0xDBFF && (i + 1) < s.size()) // 한글이면 좌표 찾기 ( codePoint )
        {
            uint32_t wc2 = (uint32_t)s[i + 1];
            if (wc2 >= 0xDC00 && wc2 <= 0xDFFF)
            {
                uint32_t high = wc - 0xD800;
                uint32_t low = wc2 - 0xDC00;
                uint32_t cp = (high << 10) + low + 0x10000;
                out.push_back(cp);
                ++i;
                continue;
            }
        }

        out.push_back(wc);
    }
}

// 라인 짜르기
static void SplitLines(const std::vector<uint32_t>& cps,
    std::vector<std::pair<size_t, size_t>>& lines)
{
    lines.clear();
    size_t start = 0;
    for (size_t i = 0; i < cps.size(); ++i)
    {
        if (cps[i] == (uint32_t)L'\n') // 글자에 줄 바꿈 문자가 있으면 쪼개기 ( lines 컨테이너에 push )
        {
            lines.push_back({ start, i }); // [start, i)
            start = i + 1;
        }
    }
    lines.push_back({ start, cps.size() });
}

void UIManager::Init(const ComPtr<ID3D11Device>& dev, const ComPtr<ID3D11DeviceContext>& ctx)
{
    device = dev;
    context = ctx;
}

void UIManager::SetSize(int w, int h)
{
    width = w;
    height = h;

    screenProj = XMMatrixOrthographicOffCenterLH(
        0.0f, (float)width,		// left, right
        (float)height, 0.0f,	// bottom, top
        0.0f, 1.0f				// near, far
    );
}

Vector2 UIManager::GetSize()
{
    return { static_cast<float>(width), static_cast<float>(height) };
}

Matrix UIManager::GetProjection() const
{
    return screenProj;
}

void UIManager::LoadFontAtlas(const std::wstring fontFilePath, TextResource* resource)
{
    if (fontFilePath.empty()) return;

    resource->atlas = builder.BuildASCII(device.Get(), fontFilePath, 
        resource->fontPx,
        resource->atlasW, resource->atlasH,
        resource->paddingPx);
}

void UIManager::RebuildGeometry(std::wstring path,
                                std::wstring text, 
                                Vector2& offset,
                                HAlign alignType,
                                TextResource* resource, 
                                std::vector<UIQuadVertex>& cpuVerts, 
                                int& outIndexCount)
{
    if (!resource) return;

    cpuVerts.clear();
    int indexCount = 0;

    std::vector<uint32_t> cps;
    DecodeUTF16ToCodepoints(text, cps);

    // (한글 지원) 아틀라스에 필요한 글리프 확보
    EnsureAtlasForText(device, resource, path, cps);
    if (!resource->atlas.srv) return;

    std::vector<std::pair<size_t, size_t>> lines;
    SplitLines(cps, lines);

    // 폰트 메트릭: ascent/lineHeight는 atlas에 있어야 함
    const float ascent = (float)resource->atlas.ascentPx;
    const float lineH = (float)resource->atlas.lineHeightPx;

    int glyphCount = 0;

    float penY = ascent; // "로컬 top=0" 기준 baseline
    for (auto [lb, le] : lines)
    {
        float lineW = MeasureWidthCP(resource, cps, lb, le);
        float offsetX = 0.0f;

        if (alignType == HAlign::Center) offsetX = (offset.x - lineW) * 0.5f;
        else if (alignType == HAlign::Right) offsetX = (offset.x - lineW);

        float penX = offsetX;
        float baselineY = penY;

        for (size_t i = lb; i < le; ++i)
        {
            uint32_t cp = cps[i];

            auto it = resource->atlas.glyphs.find(cp);
            if (it == resource->atlas.glyphs.end()) continue;

            const auto& g = it->second;

            if (g.w == 0 || g.h == 0)
            {
                penX += (float)g.advance;
                continue;
            }

            AppendGlyphQuad(penX, baselineY, cpuVerts, g);
            ++glyphCount;
            penX += (float)g.advance;
        }

        penY += lineH;
    }

    indexCount = glyphCount * 6;
    outIndexCount = indexCount;
}

void UIManager::EnsureAtlasForText(ComPtr<ID3D11Device>& dev, 
    TextResource* resource,
    std::wstring path, 
    const std::vector<uint32_t>& cps)
{
    std::vector<uint32_t> need;
    need.reserve(cps.size());

    for (uint32_t cp : cps)
    {
        if (cp == (uint32_t)L'\n') continue;
        if (resource->atlas.glyphs.find(cp) == resource->atlas.glyphs.end())
            need.push_back(cp);
    }

    if (need.empty()) return;

    // 현재 텍스트에 필요한 cp 전체를 굽는 방식(간단/안전)
    // + 필요하면 ASCII도 같이
    resource->atlas = builder.BuildFromCodepoints(
        dev.Get(),
        path,
        resource->fontPx,
        resource->atlasW, resource->atlasH,
        cps,
        resource->paddingPx,
        true
    );
}

float UIManager::MeasureWidthCP(TextResource* resource, const std::vector<uint32_t>& cps, size_t b, size_t e)
{
    float w = 0;
    for (size_t i = b; i < e; ++i)
    {
        uint32_t cp = cps[i];
        auto it = resource->atlas.glyphs.find(cp);
        if (it == resource->atlas.glyphs.end()) continue;
        w += (float)it->second.advance;
    }
    return w;
}

void UIManager::AppendGlyphQuad(float penX, float baselineY, std::vector<UIQuadVertex>& cpuVerts, const GlyphInfo& g)
{
    // y-down 좌표계 기준:
    // top = baseline - bearingY (bearingY가 baseline->top으로 +인 메트릭일 때)
    float x0 = penX + (float)g.bearingX;
    float y0 = baselineY - (float)g.bearingY;
    float x1 = x0 + (float)g.w;
    float y1 = y0 + (float)g.h;

    cpuVerts.push_back({ Vector3(x0,y0,0), Vector2(g.u0,g.v0) });
    cpuVerts.push_back({ Vector3(x1,y0,0), Vector2(g.u1,g.v0) });
    cpuVerts.push_back({ Vector3(x0,y1,0), Vector2(g.u0,g.v1) });
    cpuVerts.push_back({ Vector3(x1,y1,0), Vector2(g.u1,g.v1) });
}