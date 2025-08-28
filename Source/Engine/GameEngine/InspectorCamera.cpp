#include <InputHandler.h>

#include "InspectorCamera.h"

using namespace CommonUtilities;

void InspectorCamera::Update(const float aTimeDelta)
{
	float rotX = 0;
	float rotY = 0;

	rotX = static_cast<float>(InputHandler::GetInstance()->GetMouseDelta().y);
	rotY = static_cast<float>(InputHandler::GetInstance()->GetMouseDelta().x);

	rotation.x = std::clamp(rotation.x + rotX * mySensetivity.y, -90.f, 90.f);
	rotation.y = rotation.y + rotY * mySensetivity.x;

	const int fwAmount = InputHandler::GetInstance()->GetKey(keycode::W) - InputHandler::GetInstance()->GetKey(keycode::S);
	const int lrAmount = InputHandler::GetInstance()->GetKey(keycode::D) - InputHandler::GetInstance()->GetKey(keycode::A);
	const int udAmount = InputHandler::GetInstance()->GetKey(keycode::SPACE) - InputHandler::GetInstance()->GetKey(keycode::C);

	bool tra = InputHandler::GetInstance()->GetKey(keycode::CONTROL);

	CommonUtilities::Vector3<float> movement = CommonUtilities::Vector3<float>(
		static_cast<float>(fwAmount) * myForward + 
		static_cast<float>(lrAmount) * myRight + 
		static_cast<float>(udAmount) * myUp) *
		aTimeDelta * myCameraSpeed * 
		(myShifting ? 5 : tra ? 0.1f : 1.f);

	position += movement;

	myShifting = InputHandler::GetInstance()->GetKey(keycode::SHIFT);
	GameCamera::Update(aTimeDelta);
}