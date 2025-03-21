#include "ColorFunctions.hlsli"
#include "BRDF_Functions.hlsli"

SamplerState LUTSampler : register(s2);

float NormalDistributionFunction_GGX(float aRoughness, float aNdotH)
{
    const float roughnessSquare = aRoughness * aRoughness;
    const float denominator = aNdotH * aNdotH * (roughnessSquare - 1) + 1;
    return roughnessSquare / (PI * denominator * denominator);
}

float GeometricAttenuation_Smith_GGX(float aNdotV, float aRoughness)
{
    aNdotV = saturate(aNdotV);
    const float roughnessRemap = ((aRoughness + 1) * (aRoughness + 1)) / 8;
    return aNdotV / (aNdotV * (1 - roughnessRemap) + roughnessRemap);
}

float GeometricAttenuation_Schlick_GGX(float aNdotV, float aNdotL, float aRoughness)
{
    return GeometricAttenuation_Smith_GGX(saturate(aNdotV), aRoughness) * GeometricAttenuation_Smith_GGX(saturate(aNdotL), aRoughness);
}

float3 Fresnel_SphericalGaussianSchlick(float3 aToView, float3 aHalfangle, float3 aSpecularColour)
{
    const float viewDotHalfangle = saturate(dot(aToView, aHalfangle));
    const float p = (-5.55473 * viewDotHalfangle - 6.98316) * viewDotHalfangle;
    float3 result = aSpecularColour;
    result += (1 - aSpecularColour) * pow(2, p);
    return result;
}

float3 Specular_BRDF(float aRoughness, float3 aNormal, float3 aHalfAngle, float3 aToView, float3 aToLight, float3 aSpecularColour)
{
    const float normalDistribution = NormalDistributionFunction_GGX(aRoughness, dot(aNormal, aHalfAngle));
    const float geometricAttenuation = GeometricAttenuation_Schlick_GGX(dot(aNormal, aToView), dot(aNormal, aToLight), aRoughness);
    const float3 fresnel = Fresnel_SphericalGaussianSchlick(aToView, aHalfAngle, aSpecularColour);
    const float denominator = 4 * dot(aNormal, aToLight) * dot(aNormal, aToView);

    return normalDistribution * geometricAttenuation * fresnel / denominator;
}

float3 Diffuse_BRDF(float3 aDiffuseColour, float aRoughness)
{
    return aDiffuseColour * PIThroughOne * (1.0f - 0.5f * aRoughness);
}

float3 Diffuse_IBL(float3 aNormal)
{
    int mipCount = GetMipCount(skyBox) - 1;
    return skyBox.SampleLevel(textureSampler, aNormal, mipCount).rgb;
}

float3 Specular_IBL(float3 aNormal, float3 aToView, float aRoughness, float3 aSpecularColour)
{
    int mipCount = GetMipCount(skyBox) - 1;
    const float3 reflection = reflect(-aToView, aNormal);

    const float3 environmentColour = skyBox.SampleLevel(textureSampler, reflection, aRoughness * mipCount).rgb;

    const float nDotV = saturate(dot(aNormal, aToView));
    const float2 brdfLUT = LUTTexture.Sample(LUTSampler, float2(nDotV, aRoughness)).rg;

    return environmentColour * (aSpecularColour * brdfLUT.r + brdfLUT.g);
}

float3 EvaluateDirectionalLight(float3 albedoColor, float3 specularColor, float roughness, float metalness, float3 normal,
    float3 toLight, float3 toEye, float3 lightColor, float lightIntensity, out float intensity)
{
    float3 halfAngle = normalize(toLight + toEye);
    
    float ndl = 1 - saturate(dot(normal, -toLight));
    
    intensity = ndl * lightIntensity; // Ensure a minimum intensity
    
    // Separate intensity for diffuse and specular contributions
    const float3 lightColorAndIntensity = intensity;
    
    // Specular contribution (Fresnel-Schlick approximation)
    float3 kS = Specular_BRDF(roughness, normal, halfAngle, toEye, toLight, specularColor);
    
    // Diffuse contribution
    float3 kD = Diffuse_BRDF(albedoColor, roughness);
    kD *= (1.f - kS);
    
    // Ambient light: Adjust based on material properties and ensure it's not too overpowering
    float3 ambient = 0.3f * albedoColor; // Lower ambient intensity based on albedo for more natural lighting

    // Final lighting calculation
    return ((kD + kS) * lightColorAndIntensity + ambient) * lightColor;
}


float3 EvaluatePointLight(float3 lightPos, float3 albedoColor, float3 specularColor, float3 lightColor,
    float range, float3 pixelPosition, float3 pixelNormal, float intensity, float3 viewDir, float roughness, float metalness, out float outIintensity)
{
    float dst = length(lightPos - pixelPosition);
    if (range == 0 || dst > range)
        return (float3) 0.f;
        
    float3 lightDir = normalize(pixelPosition - lightPos);
    
    float3 toEye = -viewDir;
    
    float3 halfAngle = normalize(lightDir + toEye);
    
    float3 normal = -pixelNormal;
    
    float ndl = saturate(dot(normal, lightDir));
    
    float attenuationFactor = 1.f - (dst / range);
   
    outIintensity = max(ndl * intensity * attenuationFactor, 0.2f);
    
    const float3 lightColorAndIntensity = lightColor * max(ndl * intensity * attenuationFactor, 0.2f);
    
    float3 kS = Specular_BRDF(roughness, normal, halfAngle, toEye, -lightDir, specularColor);
    float3 kD = Diffuse_BRDF(albedoColor, roughness);
    kD *= (1.f - kS);
    
    return (kD + kS) * lightColorAndIntensity;
}

float3 EvaluateSpotLight(float3 diffuseColor, float3 specularColor, float3 toEye, float3 lightPosition, float3 lightDirection, float3 pixelPosition, float3 normal, float innerAngle, float outerAngle, float intensity,
    float range, float roughness, float3 lightColor, out float outIintensity)
{
    if (intensity == 0 || range == 0)
        return (float3) 0.f;
    
    
    if (intensity == 0 || range == 0)
        return (float3) 0.f;

    float3 toLight = lightPosition - pixelPosition;
    float dst = length(toLight);
    toLight = normalize(toLight);
    
    float spotFactor = dot(-lightDirection, toLight);
    
    float innerCone = cos(radians(innerAngle));
    float outerCone = cos(radians(outerAngle));

    float attenuation = 0.f;
    if (spotFactor >= innerCone)
        attenuation = 1;
    else if (spotFactor > outerCone)
        attenuation = (spotFactor - outerCone) / (innerCone - outerCone);

    attenuation *= saturate(1.f - (dst / range)) * intensity;
    
    float ndp = max(dot(normal, toLight), 0.0);
    
    outIintensity = max(ndp * attenuation, 0.2f);

    return diffuseColor * lightColor * max(ndp * attenuation, 0.2f);
}

float3 PBLCombined(float4 position, float3 normal, float3 diffuse, float3 material)
{
    float ao = material.r;
    float rougness = material.g;
    float metallic = material.b;
    
    const float3 specularColour = lerp((float3) 0.04f, diffuse * OB_Albedo.rgb, metallic);
    const float3 diffuseColour = lerp((float3) 0.00f, diffuse * OB_Albedo.rgb, 1.f - metallic);

    const float3 toView = (float3) normalize(FB_CameraPos - position);
    const float3 toLight = (float3) normalize(-LB_DirectionalLights[0].direction.xyz);
    const float3 halfAngle = (toView + toLight) / length(toView + toLight);

    float intensityDir = 0;
    float intensityPoint = 0;
    float intensitySpot = 0;
    
    float3 dirlight = EvaluateDirectionalLight(diffuseColour, specularColour, rougness, metallic, normal, toLight, toView, LB_PointLights[0].lightData.color.rgb, LB_DirectionalLights[0].lightData.intensity, intensityDir);
    float3 pointlight = EvaluatePointLight(LB_PointLights[0].position.xyz, diffuseColour, specularColour, LB_PointLights[0].lightData.color.rgb, LB_PointLights[0].range, position.xyz, normal, LB_PointLights[0].lightData.intensity, toView, rougness, metallic, intensityPoint);
    float3 spotlight = EvaluateSpotLight(diffuseColour, specularColour, toView, LB_SpotLights[0].position.xyz, LB_SpotLights[0].direction.xyz, position.xyz, normal, LB_SpotLights[0].cutOffInner, LB_SpotLights[0].cutOffOuter, LB_SpotLights[0].lightData.intensity, LB_SpotLights[0].range, rougness, LB_SpotLights[0].lightData.color.rgb, intensitySpot);
    
    
    //const float3 cSpecular = Specular_BRDF(rougness, normal, halfAngle, toView, toLight, specularColour);
    //float3 cDiffuse = Diffuse_BRDF(diffuseColour);
    //cDiffuse *= (1.0f - cSpecular);
    
    float totalIntensity = intensityDir * intensityPoint * intensitySpot;
    float3 totalLight = dirlight * pointlight * spotlight;

    const float3 iblSpecular = Specular_IBL(normal.xyz, toView, rougness, specularColour);
    const float3 iblDiffuse = Diffuse_IBL(normal.xyz);
    const float3 cAmbience = (diffuseColour * iblDiffuse + iblSpecular) * ao;
    const float3 radiance = (cAmbience + dirlight) * intensityDir;
    
    return radiance;
    //return float4(ao, ao, ao, 1);
    //return Specular_IBL(normal.xyz, toView, rougness, specularColour);
}