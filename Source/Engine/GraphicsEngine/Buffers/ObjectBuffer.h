#pragma once

#include <Matrix/Matrix.hpp>
#include <Vector/Vector.hpp>

struct ObjectBuffer
{
	CommonUtilities::Vector4<float> objectColorID;

	CommonUtilities::Matrix4x4<float> world;
	CommonUtilities::Matrix4x4<float> rotation;
	CommonUtilities::Vector4<float> scale;

	CommonUtilities::Vector4<float> albedo;
	float metallic;
	float roughness;

	float aoStrenght;
	float normalStrength;

	float bloomStrength;

	BOOL isInstanced;

	CommonUtilities::Vector2<float> trashOB;
};