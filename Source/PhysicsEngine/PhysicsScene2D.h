#pragma once

#include <vector>

namespace non
{
	class RigidBody2D;
	class Collider2D;

	class PhysicsScene2D
	{
	public:
		PhysicsScene2D() = default;
		~PhysicsScene2D() = default;

		void AddRigidbody(RigidBody2D* aRigidBody);
		void AddCollider(Collider2D* aCollider);

		void CheckCollisionsSphere(const Collider2D* aCollider, RigidBody2D* aRigidBody);
		void CheckCollisionsBox(const Collider2D* aCollider, RigidBody2D* aRigidBody);

		void Update(float aDeltaTime);

	private:

		std::vector<RigidBody2D*> myRigidBodies;
		std::vector<Collider2D*> myColliders;
	};
}
