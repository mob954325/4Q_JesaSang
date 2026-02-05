#include <Shared.fxh>

Texture2D textAtlasTex : register(t21);
SamplerState samPoint : register(s3);
float4 main(PS_UIImage_Input input) : SV_TARGET
{
    float a = textAtlasTex.Sample(samPoint, input.TexCoord).r;
    return float4(ImageBaseColor.rgb, ImageBaseColor.a * a);
}