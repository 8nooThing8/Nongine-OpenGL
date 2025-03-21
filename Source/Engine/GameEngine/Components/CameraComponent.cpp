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
}

void CameraComponent::UpdateAlways()
{
	GameCamera::UpdateAlways();

	position = gameObject->transform->position;
	rotation = gameObject->transform->rotation;
}

void CameraComponent::RenderImGUI()
{
	ImGui::Checkbox("Camera follow rotation", &followRotation);
}
