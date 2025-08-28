#include <InputHandler.h>

#include "Inspector2DCamera.h"
#include "../GraphicsEngine/Material/Material.h"

#include <Timer.h>
#include "../Application/editor.h"

#include <algorithm>

#include <GLFW/glfw3.h>

#include "MainSingleton.h"

using namespace CommonUtilities;

void Inspector2DCamera::Use()
{
	Material::SetGlobalMatrix4x4("invViewMatrix", GetInverse());
}

void Inspector2DCamera::Update(const float aTimeDelta)
{
	if (!Editor::Get().GetIsHoveringSceneWindow())
		return;

	// if (scrollPrevious != InputHandler::GetInstance()->GetScrollWheelDelta())
	// 	scrollWheelDelta = InputHandler::GetInstance()->GetScrollWheelDelta() - scrollPrevious;
	// scrollPrevious = InputHandler::GetInstance()->GetScrollWheelDelta();

	float scrollAmount = 1.f;
	scrollAmount *= InputHandler::GetKey(keycode::SHIFT) ? 4.f : 1.f;
	scrollAmount *= InputHandler::GetKey(keycode::CONTROL) ? 0.25f : 1.f;

	float smallerDelta = (scrollWheelDelta * scrollAmount);
	float zoomAmount = (smallerDelta);
	zoom += zoomAmount * 0.001f;
	zoom = std::clamp(zoom, 0.03f, 9999999.f);

	CU::Vector2<double> mousePos;
	glfwGetCursorPos(MainSingleton::Get().myWindow, &mousePos.x, &mousePos.y);
	if (glfwGetMouseButton(MainSingleton::Get().myWindow, 1) == GLFW_PRESS)
	{
		float sqrtZoom = sqrt(zoom);

		CU::Vector2<double> mouseDelta = previousMousePos - mousePos;

		position.x -= mouseDelta.x * aTimeDelta * sqrtZoom;
		position.y -= mouseDelta.y * aTimeDelta * 1.77777777f * sqrtZoom;

		// Camera updates
		float cosX = cos(rotation.x * CommonUtilities::deg2rad);
		float cosY = cos(rotation.y * CommonUtilities::deg2rad);

		float sinX = sin(-rotation.x * CommonUtilities::deg2rad);
		float sinY = sin(rotation.y * CommonUtilities::deg2rad);

		myForward.x = cosX * sinY;
		myForward.y = -sinX;
		myForward.z = cosX * cosY;

		myRight.x = cosY;
		myRight.z = -sinY;

		myUp = myForward.Cross(myRight);
	}

	myInverse = CommonUtilities::Matrix4x4<float>();
	myInverse(1, 1) = zoom;
	myInverse(2, 2) = zoom;
	myInverse(3, 3) = zoom;
	// myInverse *= CU::Matrix4x4<float>::CreateRotation(CU::Vector4f(-rotation.x, rotation.y, rotation.z, 0) * deg2rad).GetInverse();
	myInverse.SetPosition(CU::Vector4f(position.x, -position.y, 0, 1));

	scrollWheelDelta = 0;

	previousMousePos = mousePos;
}