#include "Includes/Sprite.hlsli"
#include "Includes/PostprocessingFunctions.hlsli"

float4 main(SpriteVSToPSOutput input) : SV_Target
{
    float3 textureColor = GaussianBlur(input.Uv, ppTextures[0], float2(1, 0)).rgb;

    return float4(textureColor, 1);
}