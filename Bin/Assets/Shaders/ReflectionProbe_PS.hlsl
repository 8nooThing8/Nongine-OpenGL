#include "Includes/PBLCalculations.hlsli"

TextureCube reflectionTexture : register(t64);

float4 main(DefaultVSToPSOutput input) : SV_TARGET
{
    // Reflect the camera direction off the surface normal
    float3 worldViewDir = normalize(input.WorldPosition - FB_CameraPos);
    float3 reflectedDir = reflect(worldViewDir, normalize(input.WorldNormal));
    
    //float ratio = 1.00 / 1.52;
    //float3 refractedDir = refract(worldViewDir, normalize(input.WorldNormal), ratio);
    
    // Sample from cubemap using reflected direction
    float4 reflectionColor = GetConvolutedCubemap(reflectedDir, reflectionTexture, 0.005f, 0);
    //float4 refractionColor = GetConvolutedCubemap(refractedDir, reflectionTexture, 0.005f, 0);
    
    return reflectionColor;
    //return refractionColor;
}