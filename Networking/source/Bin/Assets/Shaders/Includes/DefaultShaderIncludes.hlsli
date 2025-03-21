struct DefaultVSToPSOutput
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : WORLDPOSITION;
    
    float4 inputPosition : VXPOSITION;
    
    float4 LightTransformed : LIGHTTRANSFORMEDPOSITION;
    
    float4x4 Color : COLOR;
    
    float2x4 UVs : UV;
    
    float3 Normal : NORMAL;
    float3 WorldNormal : NORMALWORLD;
    
    float3 Tangent : TANGENT;
    float3 WorldTangent : TANGENTWORLD;
    
    float3 BiNormal : BINORMAL;
    float3 WorldBiNormal : BINORMALWORLD;
};