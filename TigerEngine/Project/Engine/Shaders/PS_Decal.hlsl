// [ Decal Pixel Shader ]
// Decal Volume Box의 Local UV 좌표를 기반으로 Decal Texture를 샘플링하여 출력합니다.
// - Texture mapping
// - Ring Effect (TODO)

#include <shared.fxh>

// --- Texture Bind Slot ------------------
Texture2D normalTex : register(t7);
Texture2D depthTex : register(t10);
Texture2D decalTex : register(t19);

// --- Sampler Bind Slot ------------------
SamplerState samLinear : register(s0);
SamplerState samLinearClamp : register(s2);


// 링이 끝(바깥)으로 갈수록 투명해지는 정도 조절 파라미터
static const float endFadeStart01 = 0.7f;
static const float endFadeFeather01 = 0.12f;   // 0.05~0.2

float RingMask(float2 uv, float t01)
{
    // uv: [0,1], center: (0.5,0.5)
    float2 p = (uv - 0.5f) * 2.0f; // [-1,1]
    float r = length(p); // 0~sqrt(2)

    // 반경은 0~ringMaxRadius 로
    float radius = t01 * ringMaxRadius;

    // 링 마스크: abs(r - radius) < thickness
    float d = abs(r - radius);
    float ring = 1.0f - smoothstep(ringThickness, ringThickness + ringFeather, d);

    // 바깥쪽으로 갈수록 약간 페이드(선택)
    float outer = 1.0f - smoothstep(ringMaxRadius, ringMaxRadius + ringFeather, r);

    // t01이 1에 가까워질수록 0으로 떨어지게
    // endFadeStart01 ~ 1.0 구간에서 서서히 사라짐
    float endFade = 1.0f - smoothstep(endFadeStart01, endFadeStart01 + endFadeFeather01, t01);

    return ring * outer * endFade;
}

float RingMaskMulti(float2 uv, float t01)
{
    float m = 0.0f;

    // 5개의 링이 서로 간격 두고 이동
    m += RingMask(uv, frac(t01 + 0.0f));
    m += RingMask(uv, frac(t01 + 0.2f));
    m += RingMask(uv, frac(t01 + 0.4f));
    m += RingMask(uv, frac(t01 + 0.6f));
    m += RingMask(uv, frac(t01 + 0.8f));

    return saturate(m);
}

float RingMaskContinuous(float2 uv, float base01)
{
    // 링 개수(연속 파동 수)
    const int RING_COUNT = 3; // 2~5 사이 추천
    float m = 0.0f;

    [unroll]
    for (int i = 0; i < RING_COUNT; ++i)
    {
        float phase = (float) i / (float) RING_COUNT;
        m += RingMask(uv, frac(base01 + phase));
    }

    return saturate(m);
}

float4 main(PS_Position_INPUT input) : SV_TARGET
{
    float2 uvScreen = input.pos.xy / screenSize;
    float depth = depthTex.Sample(samLinearClamp, uvScreen).r;

    float4 clip;
    clip.x = uvScreen.x * 2.0f - 1.0f;
    clip.y = (1.0f - uvScreen.y) * 2.0f - 1.0f;
    clip.z = depth;
    clip.w = 1.0f;

    float4 worldH = mul(clip, invViewProjection);
    float3 worldPos = worldH.xyz / worldH.w;

    float3 local = mul(float4(worldPos, 1.0f), decalInvWorld).xyz;

    const float h = 10.0f;
    if (any(abs(local) > h))
        discard;

    float2 uvDecal = (local.xz / (2.0f * h)) + 0.5f;
    uvDecal = uvDecal * tiling + offset;

    // Decal Type
    if (decalType == 0) // TextureMap
    {
        float4 c = decalTex.Sample(samLinearClamp, uvDecal);
        c.a *= opacity;
        if (c.a <= 0.001f)
            discard;
        return c;
    }
    else // RingEffect
    {
        float t = max(time - ringStartTime, 0.0f);

        // radius speed
        float radius = t * ringSpeed;

        // maxRadius 단위로 반복 → 0~1
        float base01 = frac(radius / max(ringMaxRadius, 0.0001f));

        // 연속 링 합성
        float mask = RingMaskContinuous(uvDecal, base01);

        // 루프 경계페이드
        float cycleFade = smoothstep(0.0f, 0.05f, base01) * (1.0f - smoothstep(0.95f, 1.0f, base01));

        float a = mask * cycleFade * opacity;
        if (a <= 0.001f)
            discard;

        return float4(ringColor, a);
    }
}