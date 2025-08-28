#pragma once

#include <Vector/Vector.hpp>
#include <Matrix/Matrix4x4.hpp>
#include <array>

namespace non
{
	struct Vertex
	{
		CommonUtilities::Vector4f vertexPosition = { 0,0,0,1 };
		CommonUtilities::Matrix4x4<float> vertexColor
		{
			std::array<std::array<float, 4>, 4>{{
				{ 0.0f, 0.0f, 0.0f, 0.0f},
				{ 0.0f, 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f, 0.0f }
				}}
		};

		CommonUtilities::Vector2f UV[4]
		{
			{0, 0},
			{0, 0},
			{0, 0},
			{0, 0}
		};

		CommonUtilities::Vector3f normal = { 0, 0, 0 };
		CommonUtilities::Vector3f tangent = { 0, 0, 0 };
		CommonUtilities::Vector3f biNormal = { 0, 0, 0 };

		CommonUtilities::Vector4u boneIds = { 0, 0, 0, 0 };
		CommonUtilities::Vector4f boneWeights = { 0, 0, 0, 0 };
	};
}
