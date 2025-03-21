#include "Includes/PBLCalculations.hlsli"
#include "Includes/Sprite.hlsli"
#include "Includes/SSAOBuffer.hlsli"

#define DIFFUSE 0
#define NORMAL 1
#define POSITION 2
#define MATERIAL 3

Texture2D textures[5] : register(t20);
Texture2D ssaoTexture : register(t25);

float4 main(SpriteVSToPSOutput input) : SV_TARGET
{
    float4 diffuse = textures[DIFFUSE].Sample(textureSampler, input.Uv);
    
    if (diffuse.a < 0.05f)
    {
        discard;
    }
    
    float4 normal = textures[NORMAL].Sample(textureSampler, input.Uv);
    float4 position = textures[POSITION].Sample(textureSampler, input.Uv);
    float4 material = textures[MATERIAL].Sample(textureSampler, input.Uv);
    float ssao;
    
    uint w, h, m;
    ssaoTexture.GetDimensions(0, w, h, m);
    const float2 texel = 1.0f / float2(w, h);
    float v = 0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            float2 offset = float2(float(x), float(y)) * texel;
            v += ssaoTexture.Sample(ppSampler, input.Uv + offset).r;
        }
    }
    
    ssao = v / (4.0f * 4.0f);
    
    // use multiply for a darker feel
    material.r *= ssao;
    //material.r = min(material.r, ssao);
    
    float3 color = PBLCombined(position, normal.rgb, diffuse.rgb, material);
    
   // return float4(r, 1);
    
    return float4(color.rgb, 1.f);
    //return float4((normalViewspace.rgb + 1.0) / 2.0, 1);
}