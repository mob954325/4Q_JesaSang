#include <Shared.fxh>

Texture2D textAtlasTex : register(t21);
// SamplerState samPoint : register(s3);
SamplerState samLinear : register(s0); // test
float4 main(PS_UIImage_Input input) : SV_TARGET
{
    float a = textAtlasTex.Sample(samLinear, input.TexCoord).r;
    return float4(ImageBaseColor.rgb, ImageBaseColor.a * a);
}