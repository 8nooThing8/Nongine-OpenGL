#include "PhysicsEngine2D.h"

#include "PhysicsScene2D.h"

namespace non
{
	PhysicsEngine2D::PhysicsEngine2D() : myCurrentPhysicsScene(new PhysicsScene2D()), myGravity(0.f, 0.f) {}

	PhysicsEngine2D& PhysicsEngine2D::Get()
	{
		static PhysicsEngine2D physicsEngine;
		return physicsEngine;
	}

	void PhysicsEngine2D::Update(float aDeltaTime)
	{
		PhysicsScene2D& physicsScene = *myCurrentPhysicsScene;

		physicsScene.Update(aDeltaTime);
	}

	const CommonUtilities::Vector2f& PhysicsEngine2D::GetGravity()
	{
		return myGravity;
	}

	void PhysicsEngine2D::SetGravity(const CommonUtilities::Vector2f& aGravity)
	{
		myGravity = aGravity;
	}

	PhysicsScene2D* PhysicsEngine2D::GetPhysicsEngine()
	{
		return myCurrentPhysicsScene;
	}
}