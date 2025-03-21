
#include "ColorFunctions.hlsli"

Texture2D immidiateTexture : register(t80);
Texture2D ppTextures[20] : register(t81);

float3 Tonemap(float3 color)
{
    //if (toneMapUsage == 1)
    //{
    //    color = LinearToGamma(color);
    //}
    //if (toneMapUsage == 2)
    //{
    //    color = LinearToGamma(Tonemap_Lottes(color));
    //}
    //if (toneMapUsage == 3)
    //{
    //    color = LinearToGamma(Tonemap_ACES(color));
    //}
    //if (toneMapUsage == 4)
    //{
    //    color = Tonemap_UnrealEngine(color);
    //}

    color = Tonemap_UnrealEngine(color);
    
    return color;
}

float4 GaussianBlur(float2 uv, Texture2D textureToBlur, float2 aHorVerCheck)
{
    float texelSizex = 1.f / PP_TextureResolution.x; // Horizontal
    float texelSizey = 1.f / PP_TextureResolution.y; // Horizontal
    static const float GaussianKernel[7] = { 0.027, 0.110, 0.207, 0.312, 0.207, 0.110, 0.027 };
    uint kernelSize = 7;
    float step = (((float) kernelSize - 1.0f) / 2.0f) * -1.0f;

    float3 result = 0;

    for (uint s = 0; s < kernelSize; ++s)
    {
        {
            float2 uvs = uv + float2(texelSizex * (step + (float) s), texelSizex * (step + (float) s)) * aHorVerCheck;
            float3 color = textureToBlur.Sample(ppSampler, uvs).rgb;
            result += color * GaussianKernel[s];
        }
    }

    return float4(result, 1.0f);
}