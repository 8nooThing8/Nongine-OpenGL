#pragma once

#include "../Component.h"

#include <Vector/Vector2.hpp>
#include <Primitive/AABB3D.hpp>
#include <Primitive/Sphere.hpp>

#include "Engine/GameEngine/Components/Physics2D/RigidBody2D.h"

class PlayerMovement : public Component
{
public:
	PlayerMovement();
	~PlayerMovement() override = default;

	void Start();
	void Update(float aDeltaTime);

private:
	float cayoteeTimer;
	float cayoteeTime = 0.15f;

	float myDrag;
	float myMoveSpeed;
	//float myMoveAccSpeed;
	//float myMaxMoveSpeed;
	CU::Vector2f myVelocity;

	non::RigidBody2D* rb;

	float sendDataTimer = 0;
};