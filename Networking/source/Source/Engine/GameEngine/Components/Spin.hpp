#pragma once

#include "Component.h"
#include "GameObject.h"

class Spin : public Component
{
public:
	friend class MeshRenderer;
	friend class RenderMeshCommand;

	Spin() = default;
	~Spin() = default;

	void Update(const float aDeltaTime) override
	{
		gameObject->transform->rotation.y += rotationSpeed * aDeltaTime;
	}

	void RenderImGUI() override
	{
		ImGui::DragFloat("RotationSpeed", &rotationSpeed, 0.1f);
	}

private:
	float rotationSpeed = 0.8f;
};

