#pragma once

#include <Vector/Vector.hpp>

#include "../../GameEngine/Data/Defines.h"

struct LightData
{
    CommonUtilities::Vector4<float> color;
    float intensity;
};

struct DirectionalLightData
{
    CommonUtilities::Vector4<float> direction;

    CommonUtilities::Matrix4x4<float> lightViewMatrix;
    CommonUtilities::Matrix4x4<float> lightProjMatrix;

    LightData light;

    CommonUtilities::Vector3<float> trash;
};

struct PointLightData
{
    CommonUtilities::Vector4<float> position;

    LightData light;

    float range;

    CommonUtilities::Vector2<float> trash;
};

struct SpotLightData
{
    CommonUtilities::Vector4<float> direction;
    CommonUtilities::Vector4<float> position;

    LightData light;

    float cutOffInner;
    float cutOffOuter;

    float range;
};

struct LightBuffer
{
    DirectionalLightData directionalLights[MaxDirectionalLights];
    PointLightData pointLights[MaxPointLights];
    SpotLightData spotLights[MaxSpotLights];

	int dirLightCount;
	int pointLightCount;
	int spotLightCount;

	float trash;
};