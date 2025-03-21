#pragma once

#include "../Components/Component.h"

#include "../GameCamera.h"

class CameraComponent : public Component, public GameCamera
{
public:
	CameraComponent() = default;

	~CameraComponent() = default;

	void Start() override;

	void Update(float aDeltaTime) override;
	void UpdateAlways() override;

	void RenderImGUI() override;

	GameObject* myGameObjectToFolow;

private:
	CommonUtilities::Vector4<float> myOffset;

	bool followRotation;
};