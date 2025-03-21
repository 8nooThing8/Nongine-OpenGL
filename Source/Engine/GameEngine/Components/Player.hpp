#pragma once

#include "Component.h"
#include <InputHandler.h>

#include <Vector/Vector4.hpp>
#include "../GameCamera.h"

#include "../Engine/GameEngine/Components/Physics/CharacterController.h"

#include "GameObject.h"

class Player : public Component
{
public:
	Player() = default;
	~Player() = default;

	void Start() override
	{
		myCC = gameObject->GetComponent<CharacterController>();
	}

	void Update(float /*aDeltaTime*/) override
	{
		int fwAmount = InputHandler::GetInstance()->GetKey(keycode::W) - InputHandler::GetInstance()->GetKey(keycode::S);
		int lrAmount = InputHandler::GetInstance()->GetKey(keycode::D) - InputHandler::GetInstance()->GetKey(keycode::A);

		CommonUtilities::Vector4<float> forward = GameCamera::main->myForward;
		CommonUtilities::Vector4<float> right = GameCamera::main->myRight;

		forward.y = 0.0f;
		right.y = 0.0f;

		forward.Normalize();
		right.Normalize();

		auto vel = CommonUtilities::Vector4<float>(static_cast<float>(fwAmount) * forward + static_cast<float>(lrAmount) * right);

		CommonUtilities::Vector2f moveVec(vel.x, vel.z);

		moveVec.Normalize();

		myCC->SetVelocity(CommonUtilities::Vector3f(moveVec.x * speed, myCC->GetVelocity().y, moveVec.y * speed));

		if (myCC->IsStandingOnGround())
		{
			myCC->GetVelocity().y = 0;

			if (InputHandler::GetInstance()->GetKey(keycode::SPACE))
			{
				myCC->AddVelocity(CommonUtilities::Vector3f(0, 1000, 0));
			}
		}

		gameObject->transform->rotation.y = GameCamera::main->rotation.y * deg2rad;
	}

private:
	CharacterController* myCC;

	float speed = 300.f;
};