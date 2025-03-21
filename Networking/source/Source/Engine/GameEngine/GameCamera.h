#pragma once

#include "Camera.h"

class GameCamera : public Camera
{
public:
	GameCamera()
	{
		rotation = CommonUtilities::Vector4<float>();
		position = CommonUtilities::Vector4<float>(0, 0, 0, 1);
	}
	~GameCamera() = default;

	void SetActiveCameraAsThis() { main = this; };

	virtual void Update(float aDeltaTime) = 0;
	virtual void UpdateAlways() = 0;

	static inline GameCamera* main = nullptr;

	CommonUtilities::Vector4<float> position;
	CommonUtilities::Vector4<float> rotation;
};