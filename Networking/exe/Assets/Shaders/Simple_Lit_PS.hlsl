#include "Includes/PBLCalculations.hlsli"

float4 main(DefaultVSToPSOutput input) : SV_TARGET
{
    if (OB_Albedo.a <= 0.05)
    {
        discard;
    }
    
    float2 uvCoords = float2(input.UVs[0].x, input.UVs[1].x);
    
    float3 normal = input.WorldNormal;
    
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
        normal = GetNormalFromMap(uvCoords, input.WorldNormal, input.WorldTangent, input.WorldBiNormal);
    }
    
    float dotProduct = CalculateDotProduct(normal);
    
    
    return float4(diffTexture.rgb * OB_Albedo.rgb * dotProduct * LB_DirectionalLights[0].lightData.intensity, OB_Albedo.a * diffTexture.a);
}