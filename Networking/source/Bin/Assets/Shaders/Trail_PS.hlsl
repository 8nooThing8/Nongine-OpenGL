#include "Includes/ColorFunctions.hlsli"
#include "Includes/Particle.hlsli"

float4 main(TrailInput input) : SV_TARGET
{
    float4 diffuseColor = diffuseTexture.Sample(textureSampler, input.Uv);

    if (diffuseColor.a < 0.05f)
        discard;

    return float4(diffuseColor.rgb * OB_Albedo.rgb, diffuseColor.a * OB_Albedo.a);
}