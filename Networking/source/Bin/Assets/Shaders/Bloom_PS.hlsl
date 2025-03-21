#include "Includes/Sprite.hlsli"
#include "Includes/PostprocessingFunctions.hlsli"

Texture2D material : register(t23);

float4 main(SpriteVSToPSOutput input) : SV_Target
{
    float4 color = immidiateTexture.Sample(ppSampler, input.Uv);
    
    float aO = material.Sample(textureSampler, input.Uv).b;
       
    float luminance = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    
    //float aO = 0;
    float threshhold = 1.2f;
    
    //return float4(color.rgb, 1);

    [Flatten]
    if (luminance >= threshhold)
        return float4(color.rgb, 1);
    else if (luminance >= threshhold * 0.5f)
    {
        float fade = luminance / threshhold;
        fade = pow(fade, 5);
        return float4(color.rgb * fade, 1);
    }
    else
        return float4(0, 0, 0, 1);
}