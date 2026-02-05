#include "Shared.fxh"

PS_UIImage_Input main(VS_UIImage_Input input)
{
    PS_UIImage_Input output;

    output.Pos = mul(float4(input.Pos, 1.0f), imageWVP);
    output.TexCoord = input.TexCoord;

    return output;
}