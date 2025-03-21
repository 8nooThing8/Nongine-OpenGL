#include "../Engine.pch.h"

#include "InspectorCamera.h"

#include <UtilityFunctions.hpp>
#include <iostream>
#include <cmath>
#include <Matrix/Matrix4x4.hpp>
#include <InputHandler.h>
#include "debugapi.h"


#define SCF static_cast<float>

using namespace CommonUtilities;

void InspectorCamera::Update(const float aTimeDelta)
{
	float rotX = 0;
	float rotY = 0;

	rotX = InputHandler::GetInstance()->GetMouseDelta().y * aTimeDelta;
	rotY = InputHandler::GetInstance()->GetMouseDelta().x * aTimeDelta;

	rotation.x = std::clamp(rotation.x + rotX * mySensetivity.y, -90.f, 90.f);
	rotation.y = rotation.y + rotY * mySensetivity.x;

	const int fwAmount = InputHandler::GetInstance()->GetKey(keycode::W) - InputHandler::GetInstance()->GetKey(keycode::S);
	const int lrAmount = InputHandler::GetInstance()->GetKey(keycode::D) - InputHandler::GetInstance()->GetKey(keycode::A);
	const int udAmount = InputHandler::GetInstance()->GetKey(keycode::SPACE) - InputHandler::GetInstance()->GetKey(keycode::C);

	bool tra = InputHandler::GetInstance()->GetKey(keycode::CONTROL);

	CommonUtilities::Vector3<float> movement = CommonUtilities::Vector3<float>(
		SCF(fwAmount) * myForward + SCF(lrAmount) * myRight + SCF(udAmount) * myUp) *
		aTimeDelta * myCameraSpeed * (myShifting ? 5 : tra ? 0.1f : 1.f);

	position += movement;
}

void InspectorCamera::UpdateAlways()
{
	myShifting = InputHandler::GetInstance()->GetKey(keycode::SHIFT);

	GameCamera::UpdateAlways();
}
