#pragma once

#include <Matrix/Matrix.hpp>
#include <Vector/Vector.hpp>

#include "GameCamera.h"

class InspectorCamera : public GameCamera
{
public:
	InspectorCamera() 
	{ 
		main = this; 

		mySensetivity = { 80.f, 80.f };

		rotation = CommonUtilities::Vector4<float>();
		position = CommonUtilities::Vector4<float>(0, 0, 0, 1);
	};

	~InspectorCamera() = default;

	void Update(float aDeltaTime);
	void UpdateAlways();

	float myCameraSpeed = 25;

	bool myShifting;

	CommonUtilities::Vector2<float> mySensetivity;
};