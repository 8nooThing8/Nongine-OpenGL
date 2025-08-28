#pragma once

#include <Matrix/Matrix.hpp>
#include <Vector/Vector.hpp>

#include "GameCamera.h"

class Inspector2DCamera : public CommonUtilities::GameCamera
{
public:
	Inspector2DCamera()
	{ 
		main = this;
		rotation = CommonUtilities::Vector4<float>();
		position = CommonUtilities::Vector4<float>(0, 0, 0, 1);
		zoom = 1.f;
		zoomInAmount = 1;
		scrollWheelDelta = 0;
		scrollPrevious = 0;
	};

	~Inspector2DCamera() = default;

	void Use() override;

	void Update(float aDeltaTime);

	float zoom;
	float zoomInAmount;

	float scrollPrevious;
	float scrollWheelDelta;

	CU::Vector2<double> previousMousePos;
};