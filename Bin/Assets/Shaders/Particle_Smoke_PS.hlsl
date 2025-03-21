#include "Includes/ColorFunctions.hlsli"
#include "Includes/Particle.hlsli"

float4 main(ParticleGStoPS input) : SV_TARGET
{
    if (input.Lifetime <= 0)
    {
        discard;
        return 0;
    }
    
    float4 diffuseColor = diffuseTexture.Sample(textureSampler, input.UV);
    
    float4 finalColor = diffuseColor;

    return finalColor.rrrr * input.Color;
}