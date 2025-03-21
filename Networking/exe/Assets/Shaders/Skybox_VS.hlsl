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
};

DefaultVSToPSOutput main(DefaultVertex vertex)
{
    DefaultVSToPSOutput result;
    
    float4 vxWorldPos = mul(OB_World, vertex.Position);
    float4 vxViewPos = mul(FB_InvView, vxWorldPos);
    float4 ndcCoords = mul(FB_Projection, vxViewPos);
    
    result.Position = ndcCoords;
    
    result.inputPosition = vertex.Position.xyzw;
    
    result.WorldPosition = vxWorldPos;
    
    result.WorldNormal = mul((float3x3) OB_World, vertex.Normal);
    result.WorldTangent = mul((float3x3) OB_World, vertex.Tangent);
    result.WorldBiNormal = mul((float3x3) OB_World, vertex.BiNormal);
   
    result.Color = vertex.Color;
    result.UVs = vertex.UVs;
    result.Normal = vertex.Normal;
    result.Tangent = vertex.Tangent;
    result.BiNormal = vertex.BiNormal;
    
    return result;
}