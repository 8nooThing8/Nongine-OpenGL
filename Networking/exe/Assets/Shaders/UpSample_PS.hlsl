#include "Includes/Sprite.hlsli"
#include "Includes/PostprocessingFunctions.hlsli"

float4 main(SpriteVSToPSOutput input) : SV_Target
{
    float3 downsample;

    float2 srcTexelSize;
    
    srcTexelSize = PP_TextureResolution / PP_DownsampledTextureResolution;

    float x = srcTexelSize.x;
    float y = srcTexelSize.y;
    
    downsample = ppTextures[0].Sample(ppSampler, float2(input.Uv.x * x, input.Uv.y * y)).rgb;

    return float4(saturate(downsample), 1);
}