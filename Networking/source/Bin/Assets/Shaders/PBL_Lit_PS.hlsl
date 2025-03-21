#include "Includes/PBLCalculations.hlsli"

float4 main(DefaultVSToPSOutput input) : SV_TARGET
{
    if (OB_Albedo.a <= 0.05)
    {
        discard;
    }
    
    float2 uvCoords = float2(input.UVs[0].x, input.UVs[1].x);
    
    float3 normal = input.WorldNormal * 0.5 + (float3)0.5;
    
    float ambientLight = 1;
    
    float4 diffTexture = 1;

    if (useTexture & 1)
    {
        diffTexture = DiffuseTexture(uvCoords);
        
        if (diffTexture.a <= 0.05)
        {
            discard;
        }
    }
    
    // change default world normal to texture normal if it so request it
    if (useTexture & 2)
    {
        normal = GetNormalFromMap(uvCoords, input.WorldNormal, input.Tangent, input.BiNormal);
    }
    
    float roughness = OB_Roughness;
    float metallic = OB_Metalic;
    float ao = 0;
    
    if (useTexture & 4)
    {
       roughness = RoughnessTexture(uvCoords);
    }
    
    if (useTexture & 8)
    {
        metallic = MetallicTexture(uvCoords);
    }
    
    if (useTexture & 16)
    {
       ao = AOTexture(uvCoords);
    }
    
    float4 light = 1;
    
    float shadow = 1;
    
    float dotProduct = CalculateDotProduct(normal.xyz);
    shadow = ShadowsCalculation(input.LightTransformed, dotProduct);
    
    float3 color = PBLCombined(input.WorldPosition, normal, diffTexture.rgb, float3(ao, metallic, roughness)) * OB_BloomStrength;
    
    return float4(color * shadow, OB_Albedo.a * diffTexture.a);
}