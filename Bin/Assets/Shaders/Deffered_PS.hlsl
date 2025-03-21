#include "Includes/PBLCalculations.hlsli"

struct GBufferOutput
{
    float4 albedo : SV_TARGET0;
    float4 worldNormal : SV_TARGET1;
    float4 worldPosition : SV_TARGET2;
    float4 material : SV_TARGET3;
    float4 pixelPicker : SV_TARGET4;
    float4 holeMask : SV_TARGET5;
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
    

    
    if (isMask == 1)
    {
        gBuffer.holeMask = float4(1, 1, 1, 1);
        
        gBuffer.albedo = 0;
        gBuffer.worldNormal = 0;
        gBuffer.worldPosition = 0;
        gBuffer.material = 0;
        gBuffer.pixelPicker = 0;
        //discard;
    }
    else
    {
        gBuffer.holeMask = float4(0, 0, 0, 1);
        
        gBuffer.albedo = float4((diffuseTexture.rgb * OB_Albedo.rgb), 1);
        gBuffer.worldNormal = float4(normal, 1);
        gBuffer.worldPosition = input.WorldPosition;
        gBuffer.material = float4(ao, roughness, metallic, shadow);
        gBuffer.pixelPicker = OB_ObjectIDColor;
    }
    
    return gBuffer;
}