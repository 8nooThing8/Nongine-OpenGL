#pragma once

#include <Vector/Vector2.hpp>
#include <Vector/Vector4.hpp>

#include "GameCamera.h"

class InspectorCamera : public CommonUtilities::GameCamera
{
public:
	InspectorCamera()
	{ 
		main = this;
		mySensetivity = { 0.1f, 0.1f };
		rotation = CommonUtilities::Vector4<float>();
		position = CommonUtilities::Vector4<float>(0, 0, 0, 1);
	};

	~InspectorCamera() = default;

	void Update(float aDeltaTime);

	bool myShifting;

	float myCameraSpeed = 25;
	CommonUtilities::Vector2<float> mySensetivity;
};