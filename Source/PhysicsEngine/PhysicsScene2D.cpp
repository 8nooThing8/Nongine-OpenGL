#include "PhysicsScene2D.h"

#include "PhysicsEngine2D.h"

#include "Components/GameObject.h"
#include "Components/Physics2D/RigidBody2D.h"
#include "Components/Physics2D/Colliders/Collider2D.h"

#include "Components/Physics2D/Colliders/BoxCollider2D.h"
#include "Components/Physics2D/Colliders/SphereCollider2D.h"

#include "Intersections.hpp"

using namespace CommonUtilities;

namespace non
{
	void PhysicsScene2D::AddRigidbody(RigidBody2D *aRigidBody)
	{
		myRigidBodies.emplace_back(aRigidBody);
	}

	void PhysicsScene2D::AddCollider(Collider2D *aCollider)
	{
		myColliders.emplace_back(aCollider);
	}

	void PhysicsScene2D::CheckCollisionsSphere(const Collider2D *aCollider, RigidBody2D *aRigidBody)
	{
		const SphereCollider2D *inSphereCollider = reinterpret_cast<const SphereCollider2D *>(aCollider);
		const CU::Vector2f &pos = inSphereCollider->GetPosition();
		SimpleSphereShape sphere(CU::Vector3f(pos.x, pos.y, 0.f), inSphereCollider->GetRadius());

		for (const auto &collider : myColliders)
		{
			if (collider->gameObject->GetID() == aRigidBody->gameObject->GetID())
				continue;

			switch (collider->myShape)
			{
			case Shape::BoxShape:
			{
				const BoxCollider2D *boxCollider = dynamic_cast<const BoxCollider2D *>(collider);

				std::vector<Vector3f> boxPoints;

				CU::Vector2f position = boxCollider->GetPosition();
				float rotation = boxCollider->GetRotation();

				CU::Vector2f halfExtents2D = boxCollider->GetHalfExtents();

				CU::Vector4f halfExtents = {halfExtents2D.x, halfExtents2D.y, 0, 0};

				halfExtents = halfExtents * CU::Matrix4x4<float>::CreateRotationAroundZ(rotation);

				boxPoints.emplace_back(CU::Vector3f(position.x + halfExtents.x, position.y + halfExtents.y, -1));
				boxPoints.emplace_back(CU::Vector3f(position.x - halfExtents.x, position.y + halfExtents.y, -1));
				boxPoints.emplace_back(CU::Vector3f(position.x + halfExtents.x, position.y - halfExtents.y, -1));
				boxPoints.emplace_back(CU::Vector3f(position.x - halfExtents.x, position.y - halfExtents.y, -1));

				boxPoints.emplace_back(CU::Vector3f(position.x + halfExtents.x, position.y + halfExtents.y, 1));
				boxPoints.emplace_back(CU::Vector3f(position.x - halfExtents.x, position.y + halfExtents.y, 1));
				boxPoints.emplace_back(CU::Vector3f(position.x + halfExtents.x, position.y - halfExtents.y, 1));
				boxPoints.emplace_back(CU::Vector3f(position.x - halfExtents.x, position.y - halfExtents.y, 1));

				CU::Vector2f posit = boxCollider->GetPosition();

				SimpleBBShape otherBox(boxPoints, {posit.x, posit.y, 0});

				float penetrationDepth = IntersectSphereABB(sphere, otherBox);
			}
			break;
			case Shape::SphereShape:
			{
				const SphereCollider2D *sphereCollider = dynamic_cast<const SphereCollider2D *>(collider);

				// SimpleSphereShape otherSphere(sphereCollider->GetPosition(), sphereCollider->GetRadius());
			}
			break;
			default:
				break;
			}
		}
	}

	void PhysicsScene2D::CheckCollisionsBox(const Collider2D *aCollider, RigidBody2D *aRigidBody)
	{
		const BoxCollider2D *inBoxCollider = reinterpret_cast<const BoxCollider2D *>(aCollider);

		std::vector<Vector3f> boxPoints;
		boxPoints.resize(8);

		float rotationFirst = inBoxCollider->GetRotation();
		{
			CU::Vector2f position = inBoxCollider->GetPosition();
			CU::Vector2f halfExtents = inBoxCollider->GetHalfExtents();

			float cosTheta = std::cos(rotationFirst);
			float sinTheta = std::sin(rotationFirst);

			CU::Vector2f corners[4] = {
				{halfExtents.x, halfExtents.y},
				{-halfExtents.x, halfExtents.y},
				{halfExtents.x, -halfExtents.y},
				{-halfExtents.x, -halfExtents.y}};

			{
				CU::Vector2f rotatedCorner;

				rotatedCorner.x = corners[0].x * cosTheta - corners[0].y * sinTheta;
				rotatedCorner.y = corners[0].x * sinTheta + corners[0].y * cosTheta;

				boxPoints[0] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, -1));
				boxPoints[4] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, 1));

				rotatedCorner.x = corners[1].x * cosTheta - corners[1].y * sinTheta;
				rotatedCorner.y = corners[1].x * sinTheta + corners[1].y * cosTheta;

				boxPoints[1] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, -1));
				boxPoints[5] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, 1));

				rotatedCorner.x = corners[2].x * cosTheta - corners[2].y * sinTheta;
				rotatedCorner.y = corners[2].x * sinTheta + corners[2].y * cosTheta;

				boxPoints[2] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, -1));
				boxPoints[6] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, 1));

				rotatedCorner.x = corners[3].x * cosTheta - corners[3].y * sinTheta;
				rotatedCorner.y = corners[3].x * sinTheta + corners[3].y * cosTheta;

				boxPoints[3] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, -1));
				boxPoints[7] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, 1));
			}
		}

		CU::Vector2f posit = inBoxCollider->GetPosition();

		SimpleBBShape box(boxPoints, {posit.x, posit.y, 0});

		auto rotationAroundZ = CommonUtilities::Matrix4x4<float>::CreateRotationAroundZ(rotationFirst);

		box.normals[0] = (Vector4f(box.normals[0], 1.f) * rotationAroundZ).ToVector3();
		box.normals[1] = (Vector4f(box.normals[1], 1.f) * rotationAroundZ).ToVector3();
		box.normals[2] = (Vector4f(box.normals[2], 1.f) * rotationAroundZ).ToVector3();

		float penetrationDepth;
		bool collidedWithAnyThing = false;

		for (const auto &collider : myColliders)
		{
			if (collider->gameObject->GetID() == aRigidBody->gameObject->GetID())
				continue;

			switch (collider->myShape)
			{
			case Shape::BoxShape:
			{
				const BoxCollider2D *otherBoxCollider = reinterpret_cast<const BoxCollider2D *>(collider);

				std::vector<Vector3f> otherBoxPoints;
				otherBoxPoints.resize(8);

				float rotation = otherBoxCollider->GetRotation();
				CU::Vector2f position = otherBoxCollider->GetPosition();
				CU::Vector2f halfExtents = otherBoxCollider->GetHalfExtents();

				float cosTheta = std::cos(rotation);
				float sinTheta = std::sin(rotation);

				CU::Vector2f corners[4] = {
					{halfExtents.x, halfExtents.y},
					{-halfExtents.x, halfExtents.y},
					{halfExtents.x, -halfExtents.y},
					{-halfExtents.x, -halfExtents.y}};

				{
					CU::Vector2f rotatedCorner;

					rotatedCorner.x = corners[0].x * cosTheta - corners[0].y * sinTheta;
					rotatedCorner.y = corners[0].x * sinTheta + corners[0].y * cosTheta;

					otherBoxPoints[0] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, -1));
					otherBoxPoints[4] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, 1));

					rotatedCorner.x = corners[1].x * cosTheta - corners[1].y * sinTheta;
					rotatedCorner.y = corners[1].x * sinTheta + corners[1].y * cosTheta;

					otherBoxPoints[1] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, -1));
					otherBoxPoints[5] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, 1));

					rotatedCorner.x = corners[2].x * cosTheta - corners[2].y * sinTheta;
					rotatedCorner.y = corners[2].x * sinTheta + corners[2].y * cosTheta;

					otherBoxPoints[2] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, -1));
					otherBoxPoints[6] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, 1));

					rotatedCorner.x = corners[3].x * cosTheta - corners[3].y * sinTheta;
					rotatedCorner.y = corners[3].x * sinTheta + corners[3].y * cosTheta;

					otherBoxPoints[3] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, -1));
					otherBoxPoints[7] = (CU::Vector3f(position.x + rotatedCorner.x, position.y + rotatedCorner.y, 1));
				}

				CU::Vector2f posit = otherBoxCollider->GetPosition();
				SimpleBBShape otherBox(otherBoxPoints, {posit.x, posit.y, 0});

				otherBox.normals[0] = Vector4f(otherBox.normals[0], 1.f) * rotationAroundZ;
				otherBox.normals[1] = Vector4f(otherBox.normals[1], 1.f) * rotationAroundZ;
				otherBox.normals[2] = Vector4f(otherBox.normals[2], 1.f) * rotationAroundZ;

				penetrationDepth = IntersectABBABB(box, otherBox);
				if (penetrationDepth)
					collidedWithAnyThing = true;
			}
			break;
			case Shape::SphereShape:
			{
				const SphereCollider2D *sphereCollider = reinterpret_cast<const SphereCollider2D *>(collider);

				const CU::Vector2f &pos = sphereCollider->GetPosition();
				SimpleSphereShape sphere(CU::Vector3f(pos.x, pos.y, 0.f), sphereCollider->GetRadius());

				bool collided = IntersectSphereABB(sphere, box);
				if (collided)
					collidedWithAnyThing = true;
			}
			break;
			default:
				break;
			}
		}

		if (!collidedWithAnyThing)
		{
			const auto &transform = aRigidBody->gameObject->transform;

			CU::Vector4f position = transform->GetPosition();

			position.x += aRigidBody->GetVelocity().x;
			position.y += aRigidBody->GetVelocity().y;

			transform->SetPosition(position);
		}
		else
		{
			const auto &transform = aRigidBody->gameObject->transform;
			transform->SetPosition(transform->GetPosition() - CU::Vector4f(aRigidBody->GetVelocity().x * penetrationDepth, aRigidBody->GetVelocity().y * penetrationDepth, 0, 0));

			auto &vel = aRigidBody->GetVelocity();
			vel.y = 0;

			// std::cout << penetrationDepth << std::endl;
		}
	}

	void PhysicsScene2D::Update(float aDeltaTime)
	{
		const CU::Vector2f &gravity = PhysicsEngine2D::Get().GetGravity();

		for (auto &rigidbody : myRigidBodies)
		{
			const auto &gameObject = rigidbody->gameObject;

			CU::Vector2f &velocity = rigidbody->GetVelocity();
			velocity -= gravity * aDeltaTime;

			if (velocity.x != 0.0f)
			{
				const float dragX = std::max(rigidbody->GetDrag(), gravity.x);
				const float dragStepX = dragX * aDeltaTime;
				if (std::abs(velocity.x) <= dragStepX)
					velocity.x = 0.0f;
				else
					velocity.x -= std::copysign(dragStepX, velocity.x);
			}

			if (velocity.y != 0.0f)
			{
				const float dragY = std::max(rigidbody->GetDrag(), gravity.y);
				const float dragStepY = dragY * aDeltaTime;
				if (std::abs(velocity.y) <= dragStepY)
					velocity.y = 0.0f;
				else
					velocity.y -= std::copysign(dragStepY, velocity.y);
			}

			const Collider2D *collider = rigidbody->gameObject->GetComponent<Collider2D>(true);
			if (!collider)
			{
				CU::Vector4f position = gameObject->transform->GetPosition();

				position.x += velocity.x;
				position.y += velocity.y;

				gameObject->transform->SetPosition(position);

				continue;
			}

			switch (collider->myShape)
			{
			case Shape::BoxShape:
			{
				CheckCollisionsBox(collider, rigidbody);
			}
			break;
			case Shape::SphereShape:
			{
				CheckCollisionsSphere(collider, rigidbody);
			}
			break;
			default:
				break;
			}
		}
	}
}