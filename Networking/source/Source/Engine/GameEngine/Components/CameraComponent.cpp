#include "CameraComponent.h"

#include "GameObject.h"

#include <InputHandler.h>

#include <ImGui\imgui.h>

void CameraComponent::Start()
{
	if (followRotation)
	{
		InputHandler::GetInstance()->SetMousePositionNormalized(0.5f, 0.5f);
		InputHandler::GetInstance()->ChangeMouseLockMode(InputHandler::Confined);
	}
}

void CameraComponent::Update(float aDeltaTime)
{
	float rotX = 0;
	float rotY = 0;

	rotX = InputHandler::GetInstance()->GetMouseDelta().y * aDeltaTime;
	rotY = InputHandler::GetInstance()->GetMouseDelta().x * aDeltaTime;

	if (InputHandler::GetInstance()->GetKeyDown(keycode::ESCAPE))
	{
		followRotation = !followRotation;

		if (followRotation)
		{
			InputHandler::GetInstance()->ChangeMouseLockMode(InputHandler::Confined);
			InputHandler::GetInstance()->ShowMyCursor(false);
		}
		else
		{
			InputHandler::GetInstance()->ChangeMouseLockMode(InputHandler::Unconfined);
			InputHandler::GetInstance()->ShowMyCursor(true);
		}
	}

	if (followRotation)
	{
		gameObject->transform->rotation.x = std::clamp(rotation.x + rotX * 50.f, -70.f, 80.f);
		gameObject->transform->rotation.y = rotation.y + rotY * 50.f;

		gameObject->transform->position = myGameObjectToFolow->transform->position + CommonUtilities::Vector4<float>(myForward * -100.f, 0) + CommonUtilities::Vector4<float>(0, 130, 0, 0);
	}
}

void CameraComponent::UpdateAlways()
{
	position = gameObject->transform->position;
	rotation = gameObject->transform->rotation;

	float cosX = cos(DegToRad(rotation.x));
	float cosY = cos(DegToRad(rotation.y));

	float sinX = sin(DegToRad(-rotation.x));
	float sinY = sin(DegToRad(rotation.y));

	myForward.x = cosX * sinY;
	myForward.y = -sinX;
	myForward.z = cosX * cosY;

	myRight.x = cosY;
	myRight.z = -sinY;

	myUp = myForward.Cross(myRight);

	float zoom = myProjectionMatrix(1, 1);

	float aspectRatioScalar = GraphicsEngine::Get().myGameResolution.x / GraphicsEngine::Get().myGameResolution.y;

	myProjectionMatrix(2, 2) = zoom * aspectRatioScalar;
}

void CameraComponent::RenderImGUI()
{
	ImGui::Checkbox("Camera follow rotation", &followRotation);
}
