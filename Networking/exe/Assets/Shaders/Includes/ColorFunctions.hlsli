#include "DefaultShaderIncludes.hlsli"
#include "FrameBuffer.hlsli"
#include "LightBuffer.hlsli"
#include "ObjectBuffer.hlsli"
#include "Defaultconstant.hlsli"
#include "Tonemap.hlsli"
#include "DebugBuffer.hlsli"
#include "PostProcessBuffer.hlsli"
#include "SpriteEditorBuffer.hlsli"

#define PI 3.14159265358979323846f
#define PIThroughOne 0.3183098861838f

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);

Texture2D roughnessTexture : register(t2);
Texture2D metallicTexture : register(t3);
Texture2D aoTexture : register(t4);

TextureCube skyBox : register(t32);

SamplerState textureSampler : register(s0);

SamplerComparisonState shadowSampler : register(s8);
Texture2D lightDepthTexture : register(t16);
Texture2D LUTTexture : register(t33);

SamplerState ppSampler : register(s5);

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

float4 DiffuseTexture(float2 uv)
{
    float4 texturePos = diffuseTexture.Sample(textureSampler, uv);
    return texturePos;
}

float3 GetNormalFromMap(float2 texCoord, float3 normal, float3 tangent, float3 binormal)
{
    float3 normalMap = normalTexture.Sample(textureSampler, texCoord).rgb;

    normalMap.b = sqrt(1 - pow(normalMap.r, 2) - pow(normalMap.g, 2));

    normalMap = normalize(normalMap * 2.0 - 1.0); // Convert from 0-1 range to -1 to 1 range

    // Reconstruct the perturbed normal
    float3 perturbedNormal = normalMap.x * tangent + normalMap.y * binormal + normalMap.z * normal;

    return normalize(perturbedNormal);
}

float RoughnessTexture(float2 uv)
{
    return roughnessTexture.Sample(textureSampler, uv).g;
}

float MetallicTexture(float2 uv)
{
    return metallicTexture.Sample(textureSampler, uv).b;
}

float AOTexture(float2 uv)
{
    return aoTexture.Sample(textureSampler, uv).r;
}

float CalculateDotProduct(float3 vertexNormal)
{
    float4 norm = normalize(float4(vertexNormal, 0));
    
    float dotValue = dot(-LB_DirectionalLights[0].direction.xyz, normalize(vertexNormal));
    return dotValue * 0.5f + 0.5f;
}

//float4 DirectionalLightCalc(float3 vertexNormal)
//{
//    float4 outputColor = LB_Color;
//    float dotProduct = CalculateDotProduct(vertexNormal);
    
//    float4 shadowedColor = lerp(outputColor * 0.5f, outputColor, dotProduct);
    
//    return shadowedColor;
//}

//float4 PointLightCalc(float4 vertexPosition, float3 vertexNormal)
//{
//    float distanceFromLight = distance(vertexPosition.xyz, LB_Position.xyz);

//    float lightConstant = 1.f;
//    float lightLinear = 0.0007f;
//    float lightQuadratic = 0.00005f;
    
//    float attenuation = 1.0 / (lightConstant + lightLinear * distanceFromLight + lightQuadratic * (distanceFromLight * distanceFromLight));
    
//    float3 lightDirection = normalize(LB_Position.xyz - vertexPosition.xyz);
    
//    float dotProduct = max(dot(vertexNormal, lightDirection), 0.0f);
    
//    // Calculate the final color
//    float4 shadowedColor = attenuation * dotProduct * LB_Color;

//    return shadowedColor;
//}

//float4 SpotLightCalc(float4 position, float3 normal)
//{
//    float3 lightDir = normalize(LB_Position.xyz - position.xyz);
    
//    float theta = dot(lightDir, normalize(-LB_Direction.xyz));

//    const float epsilon = .8f;
//    float outerCutoff = LB_CutOffInner - epsilon;
    
//    float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
    
//    if (theta > outerCutoff)
//    {
//        return float4(1.f, 1.f, 1.f, 1.f) * intensity * CalculateDotProduct(normal);
//    }
//    else
//    {
//        return float4(0.f, 0.f, 0.f, 1.f);
//    }
//}

float CalculateShadowFactor(float2 shadowTexCoords, float pixelDepth, float bias)
{
    int height;
    int width;
    
    lightDepthTexture.GetDimensions(width, height);
    
    const int kernelSize = 2;
    const float kernelRadius = 1.f / width;

    float shadowFactor = 0.0f;
    int samples = 0;
    
    for (int x = -kernelSize; x <= kernelSize; ++x)
    {
        for (int y = -kernelSize; y <= kernelSize; ++y)
        {
            float2 offset = float2(x, y) * kernelRadius;
            shadowFactor += lightDepthTexture.SampleCmpLevelZero(shadowSampler, shadowTexCoords + offset, pixelDepth - bias);
            samples++;
        }
    }
    
    shadowFactor /= samples;
    return shadowFactor;
}

float ShadowsCalculation(float4 lightSpaceVertexPos, float dotProduct)
{
    float2 shadowTexCoords;
    shadowTexCoords.x = 0.5f + (lightSpaceVertexPos.x / lightSpaceVertexPos.w * 0.5f);
    shadowTexCoords.y = 0.5f - (lightSpaceVertexPos.y / lightSpaceVertexPos.w * 0.5f);
    float pixelDepth = lightSpaceVertexPos.z / lightSpaceVertexPos.w;

    float shadowFactor = 1.0f;
    
    if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) &&
        (saturate(shadowTexCoords.y) == shadowTexCoords.y) &&
        (pixelDepth > 0))
    {
        float epsilon = 0.0000026f;
        
        float bias = epsilon * tan(acos(dotProduct));
        
        shadowFactor = CalculateShadowFactor(shadowTexCoords, pixelDepth, bias);
        //shadowFactor = lightDepthTexture.SampleCmpLevelZero(shadowSampler, shadowTexCoords, pixelDepth - bias);
        
        shadowFactor = max(shadowFactor, 0.1f);
    }
    
    return shadowFactor;
}

int GetMipCount(TextureCube aCube)
{
    int w = 0;
    int h = 0;
    int m = 0;
    aCube.GetDimensions(0, w, h, m);
    return m;
}

float4 GetConvolutedCubemap(float3 position, TextureCube cubemapToConvolut, float kernelSize, int mipLevel)
{
    float3 texCoords = position;
    
    // Convolution function: sample neighboring texels
    float3 sampleOffsets[6] =
    {
        float3(kernelSize, 0.0, 0.0),
        float3(-kernelSize, 0.0, 0.0),
        float3(0.0, kernelSize, 0.0),
        float3(0.0, -kernelSize, 0.0),
        float3(0.0, 0.0, kernelSize),
        float3(0.0, 0.0, -kernelSize)
    };

    // Base sample color (the original texel)
    float3 result = cubemapToConvolut.SampleLevel(textureSampler, texCoords, mipLevel).rgb;

    // Iterate over the offsets and sample neighboring texels
    for (int i = 0; i < 6; ++i)
    {
        float3 offset = texCoords + sampleOffsets[i];
        result += cubemapToConvolut.SampleLevel(textureSampler, offset, mipLevel).rgb;
    }

    // Average the results
    result /= 7.0f; // 1 center texel + 6 neighboring texels

    // Output the final convoluted color
    return float4(result, 1.0f);
}