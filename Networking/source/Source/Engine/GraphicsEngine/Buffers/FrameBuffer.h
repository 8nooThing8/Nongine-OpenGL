#pragma once

#include <Matrix/Matrix.hpp>

struct FrameBuffer
{
	CommonUtilities::Matrix4x4<float> InvView;
	CommonUtilities::Matrix4x4<float> Projection;
	CommonUtilities::Vector4<float> CamRot;

	CommonUtilities::Vector4<float> CameraPos;

	CommonUtilities::Vector3<float> EyeDir;

	float aspectRatio;
};