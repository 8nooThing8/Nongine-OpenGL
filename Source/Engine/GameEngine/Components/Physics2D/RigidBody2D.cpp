#include "RigidBody2D.h"

#include "Colliders/Collider2D.h"

#include "PhysicsEngine/PhysicsEngine2D.h"
#include "PhysicsEngine/PhysicsScene2D.h"

namespace non
{
	void RigidBody2D::Start()
	{
		PhysicsScene2D& physScene = *PhysicsEngine2D::Get().GetPhysicsEngine();
		physScene.AddRigidbody(this);
	}

	const CommonUtilities::Vector2f& RigidBody2D::GetVelocity() const
	{
		return myVelocity;
	}

	CommonUtilities::Vector2f& RigidBody2D::GetVelocity()
	{
		return myVelocity;
	}

	void RigidBody2D::SetVelocity(const CommonUtilities::Vector2f& aVelocity)
	{
		myVelocity = aVelocity;
	}

	float RigidBody2D::GetDrag()
	{
		return myDrag;
	}

	void RigidBody2D::SetDrag(float aDrag)
	{
		myDrag = aDrag;
	}
}
