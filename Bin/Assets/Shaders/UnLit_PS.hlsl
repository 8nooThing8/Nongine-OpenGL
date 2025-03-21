#include "Includes/ColorFunctions.hlsli"

float4 main(DefaultVSToPSOutput input) : SV_TARGET
{
    if (OB_Albedo.a <= 0.05)
    {
        discard;
    }
    
    float2 uvCoords = float2(input.UVs[0].x, input.UVs[1].x);
    
    float3 normal = input.WorldNormal;
    
    float ambientLight = 1;
    
    float4 diffuseTexture = 1;

    if (useTexture & 1)
    {
        diffuseTexture = DiffuseTexture(uvCoords);
        
        if (diffuseTexture.a <= 0.05)
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
    
    float shadow = 1;

    shadow = ShadowsCalculation(input.LightTransformed, dotProduct);

    // Combine the default light with the detail light
    float4 finalColor = max(shadow, float4(0.25f, 0.25f, 0.25f, 1)) * diffuseTexture;
    
    if (finalColor.a <= 0.05)
    {
        discard;
    }
    
    //float3 returnColor = LinearToGamma(float3(finalColor.rgb));

    return float4(diffuseTexture.rgb, 1);
}