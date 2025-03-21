#include "Includes/Sprite.hlsli"
#include "Includes/ColorFunctions.hlsli"

float4 main(SpriteVSToPSOutput input) : SV_TARGET
{
    float4 diffuse = DiffuseTexture(input.Uv);
    
    if (diffuse.a * OB_Albedo.a <= 0.05)
    {
        discard;
    }
    
    return diffuse * OB_Albedo;
}