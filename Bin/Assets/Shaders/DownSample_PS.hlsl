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
    
    //float3 b = ppTextures[0].Sample(ppSampler, float2(input.Uv.x, input.Uv.y + y)).rgb;

    //float3 d = ppTextures[0].Sample(ppSampler, float2(input.Uv.x - x, input.Uv.y)).rgb;
    //float3 e = ppTextures[0].Sample(ppSampler, float2(input.Uv.x, input.Uv.y)).rgb;
    //float3 f = ppTextures[0].Sample(ppSampler, float2(input.Uv.x + x, input.Uv.y)).rgb;
    
    //float3 h = ppTextures[0].Sample(ppSampler, float2(input.Uv.x, input.Uv.y - y)).rgb;

    //float3 j = ppTextures[0].Sample(ppSampler, float2(input.Uv.x - x, input.Uv.y + y)).rgb;
    //float3 k = ppTextures[0].Sample(ppSampler, float2(input.Uv.x + x, input.Uv.y + y)).rgb;
    //float3 l = ppTextures[0].Sample(ppSampler, float2(input.Uv.x - x, input.Uv.y - y)).rgb;
    //float3 m = ppTextures[0].Sample(ppSampler, float2(input.Uv.x + x, input.Uv.y - y)).rgb;

    //// Apply weighted distribution:
    //// 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    //// a,b,d,e * 0.125
    //// b,c,e,f * 0.125
    //// d,e,g,h * 0.125
    //// e,f,h,i * 0.125
    //// j,k,l,m * 0.5
    //// This shows 5 square areas that are being sampled. But some of them overlap,
    //// so to have an energy preserving downsample we need to make some adjustments.
    //// The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    //// contribute 0.5 to the final color output. The code below is written
    //// to effectively yield this sum. We get:
    //// 0.125*5 + 0.03125*4 + 0.0625*4 = 1

    //downsample = e * 0.125;
    ////downsample += (a + c + g + i) * 0.03125;
    //downsample += (b + d + f + h) * 0.0625;
    //downsample += (j + k + l + m) * 0.125;
    
    //downsample = saturate(downsample);

    return float4(saturate(downsample), 1);
}