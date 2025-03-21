#include "Includes/BRDF_Functions.hlsli"
#include "Includes/Sprite.hlsli"

float4 main(SpriteVSToPSOutput input) : SV_TARGET
{
    float4 integratedBRDF = IntegrateBRDF(input.Uv.x, input.Uv.y);
    return integratedBRDF;
}
