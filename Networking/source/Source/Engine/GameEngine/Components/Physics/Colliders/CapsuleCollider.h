#pragma once

#include "Collider.h"

using namespace physx;

enum Direction
{
	Up,
	Right,
	Forward
};

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider();
	CapsuleCollider(CapsuleCollider&) = default;
	~CapsuleCollider() override = default;

	CapsuleCollider(float aRadius, float aHalfHeight, Direction aUpDirection = Up);
	CapsuleCollider(float aRadius, float aHalfHeight, CollisionLayer aCollisionLayer, Direction aUpDirection = Up);

	void UpdateTransform() const override;

	void Start() override;

	void DrawCollider() override;

	void SetRadius(float aRadius);
	void SetDiameter(float aDiameter);
	void SetHalfHeight(float aHalfHeight);
	void SetFullHeight(float aFullHeight);

	void SetParameters(float aRadius, float aHalfHeight);

private:
	float myRadius;
	float myHalfHeight;

	Direction myUpDirection;
};