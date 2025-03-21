#include "Includes/PBLCalculations.hlsli"

struct GBufferOutput
{
    float4 albedo : SV_TARGET0;
    float4 worldNormal : SV_TARGET1;
    float4 worldPosition : SV_TARGET2;
    float4 material : SV_TARGET3;
    float4 pixelPicker : SV_TARGET5;
};

GBufferOutput main(DefaultVSToPSOutput input)
{
    GBufferOutput gBuffer;
    
    float2 uvCoords = float2(input.UVs[0].x, input.UVs[1].x);
    
    float3 normal = input.WorldNormal;
    
    float ambientLight = 1;
    
    float4 diffuseTexture = 1;

    if (useTexture & 1)
    {
        diffuseTexture = DiffuseTexture(uvCoords);
        
        if (diffuseTexture.a <= 0.01)
        {
            discard;
        }
    }
    
    // change default world normal to texture normal if it so request it
    if (useTexture & 2)
    {
        normal = GetNormalFromMap(uvCoords, input.WorldNormal, input.WorldTangent, input.WorldBiNormal);
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
    float shadow = 1;


    float dotProduct = CalculateDotProduct(normal.xyz);
    shadow = ShadowsCalculation(input.LightTransformed, dotProduct);
    
    gBuffer.albedo = float4((diffuseTexture.rgb * OB_Albedo.rgb) * shadow * max(OB_BloomStrength, 1), 1.f);
    gBuffer.worldNormal = float4(normal, 1);
    gBuffer.worldPosition = input.WorldPosition;
    gBuffer.material = float4(ao, roughness, metallic, 1);
    gBuffer.pixelPicker = OB_ObjectIDColor;
    
    return gBuffer;
}