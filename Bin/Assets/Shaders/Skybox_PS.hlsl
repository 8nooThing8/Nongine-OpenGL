#include "Includes/ColorFunctions.hlsli"

float4 main(DefaultVSToPSOutput input) : SV_TARGET
{
    return skyBox.SampleLevel(textureSampler, -(float3)normalize(input.inputPosition.rgb), 0);
}