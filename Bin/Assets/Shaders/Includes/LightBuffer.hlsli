#include "Defines.hlsli"

struct LightData
{
    float4 color;
    float intensity;
};

struct DirectionalLightData
{
    float4 direction;
    
    float4x4 lightViewMatrix;
    float4x4 lightProjMatrix;
    
    LightData lightData;
    
    float3 trash;
};

struct PointLightData
{
    float4 position;
    
    LightData lightData;
    
    float range;
    
    float2 trash;
};

struct SpotLightData
{
    float4 direction;
    float4 position;
    
    LightData lightData;
    
    float cutOffInner;
    float cutOffOuter;
    
    float range;
};

cbuffer LightBuffer : register(b2)
{
    DirectionalLightData LB_DirectionalLights[MaxDirectionalLights];
    PointLightData LB_PointLights[MaxPointLights];
    SpotLightData LB_SpotLights[MaxSpotLights];
    
    int LB_DirLightCount;
    int LB_PointLightCount;
    int LB_SpotLightCount;
    
    float LB_trash;
}