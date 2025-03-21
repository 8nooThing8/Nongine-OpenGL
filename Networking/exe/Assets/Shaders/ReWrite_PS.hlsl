#include "Includes/Sprite.hlsli"
#include "Includes/ColorFunctions.hlsli"

TextureCube testTexture : register(t60);

float4 main(SpriteVSToPSOutput input) : SV_TARGET
{
    return testTexture.Sample(textureSampler, input.Position);

}