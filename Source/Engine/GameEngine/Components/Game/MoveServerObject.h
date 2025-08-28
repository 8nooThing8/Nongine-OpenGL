#pragma once

#include "../Component.h"

#include <Vector/Vector2.hpp>
#include <Primitive/AABB3D.hpp>
#include <Primitive/Sphere.hpp>

class MoveServerObject : public Component
{
public:
	MoveServerObject();
	~MoveServerObject() override = default;

	void Start() override;
	void Update(float aDeltaTime) override;

	CU::Sphere myCollider;
	CU::Vector2f myVelocity;

private:
	float myDrag;
};
