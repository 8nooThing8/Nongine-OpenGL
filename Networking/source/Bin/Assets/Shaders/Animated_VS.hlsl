#include "Includes/DefaultShaderIncludes.hlsli"
#include "Includes/ObjectBuffer.hlsli"
#include "Includes/FrameBuffer.hlsli"
#include "Includes/AnimationBuffer.hlsli"
#include "Includes/LightBuffer.hlsli"

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
    
    float4 pos = vertex.Position;
    float4 skinnedPos = 0;
    uint iBone = 0;
    float fWeight = 0;
    
    float3 rotN = vertex.Normal;
    float3 rotT = vertex.Tangent;
    float3 rotB = vertex.BiNormal;
    
    float3 skinnedNormalRot = 0;
    float3 skinnedTangentRot = 0;
    float3 skinnedBiNormalRot = 0;
        
    for (int i = 0; i < 4; i++)
    {
        iBone = vertex.BoneIDs[i];
        fWeight = vertex.BoneWeights[i];
        skinnedPos += fWeight * mul(AB_Bones[iBone], pos);
    
        skinnedNormalRot += fWeight * mul((float3x3) AB_Bones[iBone], rotN);
        skinnedTangentRot += fWeight * mul((float3x3) AB_Bones[iBone], rotT);
        skinnedBiNormalRot += fWeight * mul((float3x3) AB_Bones[iBone], rotB);
    }
    
    float4 vxWorldPos = mul(OB_World, skinnedPos);
    float4 vxViewPos = mul(FB_InvView, vxWorldPos);
    float4 ndcCoords = mul(FB_Projection, vxViewPos);
    
    result.WorldPosition = vxWorldPos;
    
    // Combine light view and projection matrices into one transformation
    float4x4 lightViewProjectionMatrix = mul(LB_DirectionalLights[0].lightProjMatrix, LB_DirectionalLights[0].lightViewMatrix);
    result.LightTransformed = mul(lightViewProjectionMatrix, vxWorldPos);
    
    result.WorldNormal = mul((float3x3) OB_World, skinnedNormalRot);
    result.WorldTangent = mul((float3x3) OB_World, skinnedTangentRot);
    result.WorldBiNormal = mul((float3x3) OB_World, skinnedBiNormalRot);
    
    result.Position = ndcCoords;

    result.inputPosition = skinnedPos;

    result.Color = vertex.Color;
    result.UVs = vertex.UVs;
    result.Normal = vertex.Normal;
    result.Tangent = vertex.Tangent;
    result.BiNormal = vertex.BiNormal;
    
    return result;
}