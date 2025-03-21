#pragma once

#include "Camera.h"

#include "Primitive/Ray.hpp"
#include "Matrix/Matrix4x4.hpp"

using namespace CommonUtilities;

class GameCamera : public Camera
{
public:
	GameCamera()
	{
		rotation = CommonUtilities::Vector4<float>();
		position = CommonUtilities::Vector4<float>(0, 0, 0, 1);
	}
	~GameCamera() = default;

	void SetActiveCameraAsThis(bool aIsInspectorCamera = false) { main = this; myIsInspectorCamera = aIsInspectorCamera; };

	virtual void Update(float aDeltaTime) = 0;
	virtual void UpdateAlways();

	Ray<float> CameraPointToRay(const Vector2f& aScreenCoords);
	Ray<float> CameraPointToRayInternal(const Vector2f& aNdc);

	const CommonUtilities::Matrix4x4<float>& GetInverse();

	static inline GameCamera* main = nullptr;

	CommonUtilities::Vector4<float> position;
	CommonUtilities::Vector4<float> rotation;

	CommonUtilities::Matrix4x4<float> myInverse;

	bool myIsInspectorCamera = true;
};