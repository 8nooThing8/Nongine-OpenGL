#include "Includes/ColorFunctions.hlsli"

struct DefaultVertex
{
    float4 Position : SV_POSITION;
    
    float4x4 Color : COLOR;
    
    float2x4 UVs : UV;
    
    float3 Normal : NORMAL;
    
    float3 Tangent : TANGENT;
    
    float3 BiNormal : BINORMAL;

    uint4 BoneIDs : BONES;
    float4 BoneWeights : BONEWEIGHTS;

    float4x4 RelativeTransform : RELATIVEXF;
};

DefaultVSToPSOutput main(DefaultVertex vertex)
{
    DefaultVSToPSOutput result;

    float4 localPosition = vertex.Position;

    result.Color = vertex.Color;

    if (OB_IsInstanced)
    {
        localPosition = mul(vertex.RelativeTransform, localPosition);
    }

    float4 vxWorldPos = mul(OB_World, localPosition);
    float4 vxViewPos = mul(FB_InvView, vxWorldPos);
    float4 ndcCoords = mul(FB_Projection, vxViewPos);

    result.WorldPosition = vxWorldPos;

    result.Position = ndcCoords;
    result.inputPosition = vertex.Position;
    
    // Combine light view and projection matrices into one transformation
    float4x4 lightViewProjectionMatrix = mul(LB_DirectionalLights[0].lightProjMatrix, LB_DirectionalLights[0].lightViewMatrix);
    result.LightTransformed = mul(lightViewProjectionMatrix, vxWorldPos);
    
    result.Normal = vertex.Normal;
    result.Tangent = vertex.Tangent;
    result.BiNormal = vertex.BiNormal;

    result.WorldNormal = mul((float3x3) OB_World, vertex.Normal);
    result.WorldTangent = mul((float3x3) OB_World, vertex.Tangent);
    result.WorldBiNormal = mul((float3x3) OB_World, vertex.BiNormal);
   
    result.UVs = vertex.UVs;
    
    return result;
}