#include "Includes/Sprite.hlsli"
#include "Includes/PostprocessingFunctions.hlsli"

float4 main(SpriteVSToPSOutput input) : SV_TARGET
{
    float4 textureColor = immidiateTexture.Sample(ppSampler, input.Uv);
    float4 newCo = SceneDepthTexture.Sample(ppSampler, input.Uv);
    
    textureColor += ppTextures[1].Sample(ppSampler, input.Uv);
	
    float3 color = Tonemap(textureColor.rgb);
    
   // color = ppTextures[3].Sample(ppSampler, input.Uv);
    
    
    //if (newCo.r >= 0.98f)
    //{
    //    return float4(1, 1, 1, 0);
    //}
        
    return float4(color.rgb, 1.f);
}