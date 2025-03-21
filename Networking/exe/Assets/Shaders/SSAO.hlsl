#include "Includes/PBLCalculations.hlsli"
#include "Includes/Sprite.hlsli"
#include "Includes/SSAOBuffer.hlsli"

#define NORMAL 1
#define POSITION 2

Texture2D textures[5] : register(t20);
Texture2D NoiseTexture : register(t19);

float4 GetViewSpacePosition(float2 UV)
{
    float4 position = textures[POSITION].SampleLevel(textureSampler, UV, 0);
    return mul(FB_InvView, position);
}

float4 GetViewSpaceNormal(float2 UV)
{
    float4 normal = textures[NORMAL].SampleLevel(textureSampler, UV, 0);
    return float4(mul((float3x3) FB_InvView, normal.xyz), 1);
}

float3 GetRandomNoise(float2 UV, float2 aNoiseScale)
{
    return NoiseTexture.SampleLevel(textureSampler, UV * aNoiseScale, 0).rgb;
}

float2 GetNoiseScale()
{
    uint w, h, m;
    NoiseTexture.GetDimensions(0, w, h, m);
    float2 noiseScale = float2(w, h);
    return PP_TextureResolution / noiseScale;
}

float4 main(SpriteVSToPSOutput input) : SV_TARGET
{
    float occlusion = 0;
    
    const float2 noiseScale = GetNoiseScale();
    const float3 pos = GetViewSpacePosition(input.Uv);
    const float3 nrm = GetViewSpaceNormal(input.Uv);
    const float noisePwr = 0.2f;
    const float3 r = GetRandomNoise(input.Uv, noiseScale) * noisePwr;
    const float3 T = normalize(r - nrm * dot(r, nrm));
    const float3 B = cross(nrm, T);
    const float3x3 TBN = float3x3(T, B, nrm);
    
    const float radius = 10.0f; // Larger radius for softer occlusion
    const float bias = 0.02f; // Slightly lower bias for smoothness
    const float smoothingFactor = 0.6f; // New smoothing factor for final output

    for (int i = 0; i < SSAO_KernelSize; ++i)
    {
        float3 kernelPos = mul(TBN, SSAO_Kernel[i].xyz);
        kernelPos = pos.xyz + kernelPos * radius;
        float4 offset = float4(kernelPos, 1);
        offset = mul(FB_Projection, offset);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5f + 0.5f;
        offset.y = 1 - offset.y;
        
        const float stepDepth = GetViewSpacePosition(offset.xy).z;
        const float range = smoothstep(0.0, 1.0, radius / abs(pos.z - stepDepth));
        occlusion += (stepDepth >= kernelPos.z + bias ? 1.0 : 0.0) * range;
    }
    
    // Averaging the occlusion and applying a smoothing factor for softer appearance
    occlusion = 1.0f - (occlusion / SSAO_KernelSize);
    occlusion = lerp(1.0f, occlusion, smoothingFactor);
    
    return float4(occlusion, occlusion, occlusion, 1.f);
}
