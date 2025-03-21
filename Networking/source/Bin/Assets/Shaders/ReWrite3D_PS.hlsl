#include "Includes/ColorFunctions.hlsli"

TextureCube testTexture : register(t60);

float4 main(DefaultVSToPSOutput input) : SV_TARGET
{
    return testTexture.Sample(textureSampler, input.inputPosition.xyz);
}